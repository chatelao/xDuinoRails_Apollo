/**
 * @file test_main.cpp
 * @brief Main file for the native unit test suite.
 *
 * This file contains the test suite for the decoder firmware, run on a native
 * environment using the Unity framework and ArduinoFake.
 */
#include <unity.h>
#include <vector>
#include <cstdint>
#include <ArduinoFake.h>

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Mocks & Includes
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

using namespace fakeit;

// Global state to mock effect activation for AuxController test.
bool effect_is_active = false;

// Custom mock for Effect used only in the AuxController test.
class MockEffect : public Effect {
public:
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override {}
    void setActive(bool active) override {
        effect_is_active = active;
    }
};

// Include the source files directly to resolve linker errors in the test environment.
#include "AuxController.cpp"
#include "CVManager.cpp"
// Include WAVStream for testing
#include "sound/WAVStream.cpp"

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Test Globals & Setup
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const uint32_t DELTA_MS = 16;
std::vector<PhysicalOutput*> mock_outputs;
Mock<PhysicalOutput> mock_output1(1, OutputType::PWM);
Mock<PhysicalOutput> mock_output2(2, OutputType::PWM);

/**
 * @brief Test setup function, called before each test case.
 * Resets mocks and global state.
 */
void setUp(void) {
    ArduinoFakeReset();
    effect_is_active = false;
    mock_outputs.clear();
    mock_outputs.push_back(&mock_output1.get());
    mock_outputs.push_back(&mock_output2.get());
    When(Method(mock_output1, setValue)).AlwaysReturn();
    When(Method(mock_output2, setValue)).AlwaysReturn();
}

/**
 * @brief Test teardown function, called after each test case.
 */
void tearDown(void) {}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Test Cases
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * @brief Test the EffectSteady class.
 * Ensures the effect sets the correct brightness when active and inactive.
 */
void test_effect_steady() {
    EffectSteady effect(128);
    effect.setActive(false);
    effect.update(DELTA_MS, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(0)).Once();

    effect.setActive(true);
    effect.update(DELTA_MS, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(128)).Once();
}

/**
 * @brief Test the EffectDimming class.
 * Ensures the effect switches between full and dimmed brightness levels correctly.
 */
void test_effect_dimming() {
    EffectDimming effect(255, 80);
    effect.setActive(true);
    effect.setDimmed(false);
    effect.update(DELTA_MS, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(255)).Once();

    effect.setDimmed(true);
    effect.update(DELTA_MS, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(80)).Once();
}

/**
 * @brief Test activating and deactivating a LogicalFunction.
 * Ensures the logical function correctly controls its underlying physical output.
 */
void test_logical_function_activation() {
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    PhysicalOutput output(1, OutputType::PWM);
    Effect* effect = new EffectSteady(200);
    LogicalFunction func(effect);
    func.addOutput(&output);

    func.setActive(false);
    func.update(16);
    Verify(Method(ArduinoFake(), analogWrite).Using(1, 0)).Once();

    func.setActive(true);
    func.update(16);
    Verify(Method(ArduinoFake(), analogWrite).Using(1, 200)).Once();
}

/**
 * @brief Test the strobe effect timing.
 * Verifies the on/off timing of the strobe effect based on its duty cycle.
 */
void test_effect_strobe() {
    EffectStrobe effect(10, 25, 255); // 10Hz, 25% duty cycle -> 100ms period, 25ms on time
    effect.setActive(true);

    effect.update(10, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(255)).Once();

    effect.update(15, mock_outputs); // Total elapsed = 25ms
    Verify(Method(mock_output1, setValue).Using(0)).Once();

    effect.update(75, mock_outputs); // Total elapsed = 100ms
    Verify(Method(mock_output1, setValue).Using(255)).Twice();
}

/**
 * @brief Test the soft start/stop fade timing.
 * Verifies the brightness levels at different points during the fade-in and fade-out cycles.
 */
void test_effect_soft_start_stop() {
    EffectSoftStartStop effect(100, 50, 200); // 100ms fade in, 50ms fade out, target 200

    effect.setActive(true);
    effect.update(50, mock_outputs); // Halfway through fade in
    Verify(Method(mock_output1, setValue).Using(100)).Once();
    effect.update(50, mock_outputs); // Fully faded in
    Verify(Method(mock_output1, setValue).Using(200)).Once();

    effect.setActive(false);
    effect.update(25, mock_outputs); // Halfway through fade out
    Verify(Method(mock_output1, setValue).Using(100)).Twice();
    effect.update(25, mock_outputs); // Fully faded out
    Verify(Method(mock_output1, setValue).Using(0)).Once();
}

/**
 * @brief Test a simple mapping rule in the AuxController.
 * Ensures a function key press correctly activates a logical function.
 */
void test_manager_mapping_rule() {
    AuxController controller;
    controller.addLogicalFunction(new LogicalFunction(new MockEffect()));

    ConditionVariable cv;
    cv.id = 1;
    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 1});
    controller.addConditionVariable(cv);

    MappingRule rule;
    rule.target_logical_function_id = 0;
    rule.positive_conditions.push_back(1);
    rule.action = MappingAction::ACTIVATE;
    controller.addMappingRule(rule);

    controller.update(DELTA_MS);
    TEST_ASSERT_FALSE(effect_is_active);

    controller.setFunctionState(1, true);
    controller.update(DELTA_MS);
    TEST_ASSERT_TRUE(effect_is_active);
}

/**
 * @brief Test that the AuxController correctly loads default RCN-225 headlight configuration from CVs.
 */
void test_cv_loader_default_headlight_config() {
    CVManager cvManager;
    AuxController auxController;
    auxController.begin();
    cvManager.begin();

    auxController.loadFromCVs(cvManager);

    TEST_ASSERT_EQUAL(2, auxController.getLogicalFunctionCount());
    TEST_ASSERT_EQUAL(3, auxController.getConditionVariableCount());
    TEST_ASSERT_EQUAL(2, auxController.getMappingRuleCount());

    LogicalFunction* lf0 = auxController.getLogicalFunction(0);
    TEST_ASSERT_NOT_NULL(lf0);
}


/**
 * @brief Test the RCN-227 "per function" mapping logic.
 */
void test_rcn227_per_function_mapping() {
    CVManager cvManager;
    AuxController auxController;
    auxController.begin();
    cvManager.begin();

    // Select RCN-227 "per function" mapping
    cvManager.writeCV(CV_FUNCTION_MAPPING_METHOD, 2);

    // --- Configure a test mapping for F1 Forward ---
    // Outputs 1 (bit 0) and 3 (bit 2) should be active. Mask = 0b00000101 = 5
    // Blocked by function F5.
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 40);
    uint16_t f1_fwd_base_cv = 257 + (1 * 2 + 0) * 4; // F1, Forward
    cvManager.writeCV(f1_fwd_base_cv, 5);      // Output mask
    cvManager.writeCV(f1_fwd_base_cv + 1, 0);  // Output mask (byte 2)
    cvManager.writeCV(f1_fwd_base_cv + 2, 0);  // Output mask (byte 3)
    cvManager.writeCV(f1_fwd_base_cv + 3, 5);  // Blocking function F5

    // Load the configuration
    auxController.loadFromCVs(cvManager);

    // --- Verification ---
    // We expect 2 logical functions (one for each output bit).
    TEST_ASSERT_EQUAL(2, auxController.getLogicalFunctionCount());
    // We expect 2 condition variables: one for (F1 & FWD), one for (F5).
    TEST_ASSERT_EQUAL(2, auxController.getConditionVariableCount());
    // We expect 2 mapping rules, one for each logical function.
    TEST_ASSERT_EQUAL(2, auxController.getMappingRuleCount());

    // --- Test Activation Logic ---
    // Set initial state: Forward direction, F1 is OFF, F5 is OFF
    auxController.setDirection(DECODER_DIRECTION_FORWARD);
    auxController.setFunctionState(1, false);
    auxController.setFunctionState(5, false);
    auxController.update(DELTA_MS);
    LogicalFunction* lf_out1 = auxController.getLogicalFunction(0);
    LogicalFunction* lf_out3 = auxController.getLogicalFunction(1);
    TEST_ASSERT_FALSE(lf_out1->isActive());
    TEST_ASSERT_FALSE(lf_out3->isActive());

    // Now, turn ON F1. Outputs should activate.
    auxController.setFunctionState(1, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_TRUE(lf_out1->isActive());
    TEST_ASSERT_TRUE(lf_out3->isActive());

    // Now, turn ON the blocking function F5. Outputs should deactivate.
    auxController.setFunctionState(5, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_FALSE(lf_out1->isActive());
    TEST_ASSERT_FALSE(lf_out3->isActive());
}

/**
 * @brief Test the RCN-227 "per output" V1 (Matrix) mapping logic.
 */
void test_rcn227_per_output_v1_mapping() {
    CVManager cvManager;
    AuxController auxController;
    auxController.begin();
    cvManager.begin();

    // Select RCN-227 "per output" V1 mapping
    cvManager.writeCV(CV_FUNCTION_MAPPING_METHOD, 3);

    // --- Configure a test mapping for Output 1 ---
    // F1 should activate it in forward direction (F1 = bit 1)
    // F2 should activate it in reverse direction (F2 = bit 2)
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 41);
    uint16_t out1_fwd_base = 257 + (0 * 2 + 0) * 4; // Output 1 (index 0), Forward
    uint16_t out1_rev_base = 257 + (0 * 2 + 1) * 4; // Output 1 (index 0), Reverse
    cvManager.writeCV(out1_fwd_base, 0b00000010); // F1
    cvManager.writeCV(out1_rev_base, 0b00000100); // F2

    // Load the configuration
    auxController.loadFromCVs(cvManager);

    // --- Verification ---
    // We expect 1 logical function for Output 1.
    TEST_ASSERT_EQUAL(1, auxController.getLogicalFunctionCount());
    LogicalFunction* lf = auxController.getLogicalFunction(0);
    TEST_ASSERT_NOT_NULL(lf);

    // --- Test Activation Logic ---
    // State: Fwd, F1 OFF, F2 OFF -> Inactive
    auxController.setDirection(DECODER_DIRECTION_FORWARD);
    auxController.setFunctionState(1, false);
    auxController.setFunctionState(2, false);
    auxController.update(DELTA_MS);
    TEST_ASSERT_FALSE(lf->isActive());

    // State: Fwd, F1 ON -> Active
    auxController.setFunctionState(1, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_TRUE(lf->isActive());

    // State: Rev, F1 ON -> Inactive
    auxController.setDirection(DECODER_DIRECTION_REVERSE);
    auxController.update(DELTA_MS);
    TEST_ASSERT_FALSE(lf->isActive());

    // State: Rev, F2 ON -> Active
    auxController.setFunctionState(2, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_TRUE(lf->isActive());
}

/**
 * @brief Test the RCN-227 "per output" V2 (Function Number) mapping logic.
 */
void test_rcn227_per_output_v2_mapping() {
    CVManager cvManager;
    AuxController auxController;
    auxController.begin();
    cvManager.begin();

    // Select RCN-227 "per output" V2 mapping
    cvManager.writeCV(CV_FUNCTION_MAPPING_METHOD, 4);

    // --- Configure a test mapping for Output 2 ---
    // Should activate with F3 OR F4 in forward direction.
    // Should be blocked by F5 (applies to both directions).
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 42);
    uint16_t out2_fwd_base = 257 + (1 * 2 + 0) * 4; // Output 2 (index 1), Forward
    cvManager.writeCV(out2_fwd_base, 3);     // F3
    cvManager.writeCV(out2_fwd_base + 1, 4); // F4
    cvManager.writeCV(out2_fwd_base + 2, 255); // Unused
    cvManager.writeCV(out2_fwd_base + 3, 5);   // Blocking F5

    // Load the configuration
    auxController.loadFromCVs(cvManager);

    // --- Verification ---
    // We expect 1 logical function for Output 2.
    TEST_ASSERT_EQUAL(1, auxController.getLogicalFunctionCount());
    LogicalFunction* lf = auxController.getLogicalFunction(0);
    TEST_ASSERT_NOT_NULL(lf);

    // --- Test Activation Logic ---
    auxController.setDirection(DECODER_DIRECTION_FORWARD);
    auxController.setFunctionState(3, false);
    auxController.setFunctionState(4, false);
    auxController.setFunctionState(5, false);

    // State: Fwd, F3 OFF, F4 OFF, F5 OFF -> Inactive
    auxController.update(DELTA_MS);
    TEST_ASSERT_FALSE(lf->isActive());

    // State: Fwd, F3 ON -> Active
    auxController.setFunctionState(3, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_TRUE(lf->isActive());
    auxController.setFunctionState(3, false); // reset

    // State: Fwd, F4 ON -> Active
    auxController.setFunctionState(4, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_TRUE(lf->isActive());

    // State: Fwd, F4 ON, F5 ON (blocking) -> Inactive
    auxController.setFunctionState(5, true);
    auxController.update(DELTA_MS);
    TEST_ASSERT_FALSE(lf->isActive());
}

/**
 * @brief Test WAVStream looping functionality.
 */
void test_wav_stream_looping() {
    WAVStream stream;

    // Create a minimal 8-bit mono WAV in memory (Header + 2 bytes data)
    // Data: 0x80 (mid), 0xFF (max)
    uint8_t wav_data[] = {
        'R', 'I', 'F', 'F',
        38, 0, 0, 0,  // Chunk Size (36 + 2)
        'W', 'A', 'V', 'E',
        'f', 'm', 't', ' ',
        16, 0, 0, 0,  // Subchunk1 Size
        1, 0,         // PCM
        1, 0,         // Mono
        68, 172, 0, 0, // 44100 Hz
        68, 172, 0, 0, // Byte Rate
        1, 0,         // Block Align
        8, 0,         // Bits per Sample
        'd', 'a', 't', 'a',
        2, 0, 0, 0,   // Subchunk2 Size
        0x80, 0xFF    // Data
    };

    TEST_ASSERT_TRUE(stream.begin(wav_data, sizeof(wav_data)));

    // Test without looping
    stream.setLooping(false);
    int16_t l, r;

    // Sample 1
    stream.get_next_sample(&l, &r);
    TEST_ASSERT_EQUAL(0, l); // 0x80 -> 0

    // Sample 2
    stream.get_next_sample(&l, &r);
    // 0xFF -> 255 - 128 = 127 -> <<8 = 32512
    TEST_ASSERT_EQUAL(32512, l);

    // End of stream
    TEST_ASSERT_TRUE(stream.is_finished());
    stream.get_next_sample(&l, &r);
    TEST_ASSERT_EQUAL(0, l);


    // Test WITH looping
    stream.rewind();
    stream.setLooping(true);

    // Sample 1
    stream.get_next_sample(&l, &r);
    TEST_ASSERT_EQUAL(0, l);

    // Sample 2
    stream.get_next_sample(&l, &r);
    TEST_ASSERT_EQUAL(32512, l);

    // Should NOT be finished
    TEST_ASSERT_FALSE(stream.is_finished());

    // Loop back to Sample 1
    stream.get_next_sample(&l, &r);
    TEST_ASSERT_EQUAL(0, l);

    TEST_ASSERT_FALSE(stream.is_finished());
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Main Test Runner
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * @brief Main setup for the test runner.
 */
void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_effect_steady);
    RUN_TEST(test_effect_dimming);
    RUN_TEST(test_logical_function_activation);
    RUN_TEST(test_effect_strobe);
    RUN_TEST(test_effect_soft_start_stop);
    RUN_TEST(test_manager_mapping_rule);
    RUN_TEST(test_cv_loader_default_headlight_config);
    RUN_TEST(test_logical_function_activation);
    RUN_TEST(test_rcn227_per_function_mapping);
    RUN_TEST(test_rcn227_per_output_v1_mapping);
    RUN_TEST(test_rcn227_per_output_v2_mapping);
    RUN_TEST(test_wav_stream_looping);
    UNITY_END();
}

/**
 * @brief Main loop for the test runner.
 */
void loop() {}
