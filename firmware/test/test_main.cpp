#include <unity.h>
#include <vector>
#include <cstdint>
#include <ArduinoFake.h>

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Mocks & Includes
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

using namespace fakeit;

// Global state to mock effect activation for FunctionManager test.
bool effect_is_active = false;

// Custom mock for Effect used only in the FunctionManager test.
class MockEffect : public Effect {
public:
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override {}
    void setActive(bool active) override {
        effect_is_active = active;
    }
};

// Include the source files directly to resolve linker errors in the test environment.
#include "Effect.cpp"
#include "PhysicalOutput.cpp"
#include "LogicalFunction.cpp"
#include "FunctionMapping.cpp"
#include "FunctionManager.cpp"
#include "CVManager.cpp"
#include "PhysicalOutputManager.cpp"
#include "CVLoader.cpp"

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Test Cases
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const uint32_t DELTA_MS = 16;
std::vector<PhysicalOutput*> mock_outputs;
Mock<PhysicalOutput> mock_output1(1);
Mock<PhysicalOutput> mock_output2(2);


void setUp(void) {
    ArduinoFakeReset();
    effect_is_active = false;
    mock_outputs.clear();
    mock_outputs.push_back(&mock_output1.get());
    mock_outputs.push_back(&mock_output2.get());
    When(Method(mock_output1, setValue)).AlwaysReturn();
    When(Method(mock_output2, setValue)).AlwaysReturn();
}

void tearDown(void) {}

// Summary: Test the EffectSteady class.
void test_effect_steady() {
    EffectSteady effect(128);
    effect.setActive(false);
    effect.update(DELTA_MS, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(0)).Once();

    effect.setActive(true);
    effect.update(DELTA_MS, mock_outputs);
    Verify(Method(mock_output1, setValue).Using(128)).Once();
}

// Summary: Test the EffectDimming class.
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

// Summary: Test activating and deactivating a LogicalFunction.
void test_logical_function_activation() {
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    PhysicalOutput output(1, PhysicalOutputType::PWM_LOW_SIDE);
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

// Summary: Test the strobe effect timing.
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

// Summary: Test the soft start/stop fade timing.
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

// Summary: Test a simple mapping rule in the FunctionManager.
void test_manager_mapping_rule() {
    FunctionManager manager;
    manager.addLogicalFunction(new LogicalFunction(new MockEffect()));

    ConditionVariable cv;
    cv.id = 1;
    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 1});
    manager.addConditionVariable(cv);

    MappingRule rule;
    rule.target_logical_function_id = 0;
    rule.positive_conditions.push_back(1);
    rule.action = MappingAction::ACTIVATE;
    manager.addMappingRule(rule);

    manager.update(DELTA_MS);
    TEST_ASSERT_FALSE(effect_is_active);

    manager.setFunctionState(1, true);
    manager.update(DELTA_MS);
    TEST_ASSERT_TRUE(effect_is_active);
}

// Summary: Test that the CVLoader correctly configures the FunctionManager with default CVs.
void test_cv_loader_default_headlight_config() {
    CVManager cvManager;
    FunctionManager functionManager;
    PhysicalOutputManager physicalOutputManager;
    physicalOutputManager.begin();
    cvManager.begin();

    CVLoader::loadCvToFunctionManager(cvManager, functionManager, physicalOutputManager);

    TEST_ASSERT_EQUAL(2, functionManager.getLogicalFunctionCount());
    TEST_ASSERT_EQUAL(3, functionManager.getConditionVariableCount());
    TEST_ASSERT_EQUAL(2, functionManager.getMappingRuleCount());

    LogicalFunction* lf0 = functionManager.getLogicalFunction(0);
    TEST_ASSERT_NOT_NULL(lf0);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Main Test Runner
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

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
    UNITY_END();
}

void loop() {}
