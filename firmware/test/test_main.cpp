#include <unity.h>
#include <vector>
#include <cstdint>
#include <ArduinoFake.h>

// =====================================================================================
// Mocks & Includes
// =====================================================================================

using namespace fakeit;

// Global state to mock effect activation for FunctionManager test.
bool effect_is_active = false;

// Custom mock for LightEffect used only in the FunctionManager test.
class MockEffect : public LightEffect {
public:
    void update(uint32_t delta_ms) override {}
    uint8_t getPwmValue() override { return 0; }
    void setActive(bool active) override {
        effect_is_active = active;
    }
};


// Include the source files directly to resolve linker errors in the test environment.
// All required source files are included here once for all tests.
#include "LightEffect.cpp"
#include "PhysicalOutput.cpp"
#include "LogicalFunction.cpp"
// FunctionManager depends on FunctionMapping, so include it first.
#include "FunctionMapping.cpp"
#include "FunctionManager.cpp"
// CV related classes are last as they depend on the others.
#include "CVManager.cpp"
#include "PhysicalOutputManager.cpp"
#include "CVLoader.cpp"


// =====================================================================================
// Test Cases
// =====================================================================================

// A mock delta time for update calls.
const uint32_t DELTA_MS = 16;

void setUp(void) {
    // Reset mocks before each test
    ArduinoFakeReset();
    effect_is_active = false;
}

void tearDown(void) {
    // Clean up code, optional.
}

/**
 * @brief Test the EffectSteady class.
 */
void test_effect_steady() {
    EffectSteady effect(128);
    effect.setActive(false);
    TEST_ASSERT_EQUAL(0, effect.getPwmValue());
    effect.setActive(true);
    effect.update(DELTA_MS);
    TEST_ASSERT_EQUAL(128, effect.getPwmValue());
}

/**
 * @brief Test the EffectDimming class.
 */
void test_effect_dimming() {
    EffectDimming effect(255, 80);
    effect.setActive(false);
    TEST_ASSERT_EQUAL(0, effect.getPwmValue());
    effect.setActive(true);
    effect.setDimmed(false);
    effect.update(DELTA_MS);
    TEST_ASSERT_EQUAL(255, effect.getPwmValue());
    effect.setDimmed(true);
    effect.update(DELTA_MS);
    TEST_ASSERT_EQUAL(80, effect.getPwmValue());
}

/**
 * @brief Test activating and deactivating a LogicalFunction.
 */
void test_logical_function_activation() {
    When(Method(ArduinoFake(), pinMode)).AlwaysReturn();
    When(Method(ArduinoFake(), analogWrite)).AlwaysReturn();

    PhysicalOutput output(1);
    EffectSteady* effect = new EffectSteady(200);
    LogicalFunction func(effect);
    func.addOutput(&output);

    // When inactive, output should be 0.
    func.setActive(false);
    func.update(16);
    Verify(Method(ArduinoFake(), analogWrite).Using(1, 0)).Once();

    // When active, output should be the effect's value.
    func.setActive(true);
    func.update(16);
    Verify(Method(ArduinoFake(), analogWrite).Using(1, 200)).Once();
}

/**
 * @brief Test the strobe effect timing.
 */
void test_effect_strobe() {
    // 10Hz strobe with 25% duty cycle = 100ms period, 25ms on time.
    EffectStrobe effect(10, 25, 255);
    effect.setActive(true);

    effect.update(10); // t=10ms
    TEST_ASSERT_EQUAL(255, effect.getPwmValue());
    effect.update(15); // t=25ms
    TEST_ASSERT_EQUAL(0, effect.getPwmValue());
    effect.update(75); // t=100ms
    TEST_ASSERT_EQUAL(255, effect.getPwmValue());
}

/**
 * @brief Test the soft start/stop fade timing.
 */
void test_effect_soft_start_stop() {
    // 100ms fade in, 50ms fade out, target 200.
    EffectSoftStartStop effect(100, 50, 200);

    // Test fade in
    effect.setActive(true);
    effect.update(50); // Halfway through fade in
    TEST_ASSERT_EQUAL(100, effect.getPwmValue());
    effect.update(50); // Fully faded in
    TEST_ASSERT_EQUAL(200, effect.getPwmValue());

    // Test fade out
    effect.setActive(false);
    effect.update(25); // Halfway through fade out
    TEST_ASSERT_EQUAL(100, effect.getPwmValue());
    effect.update(25); // Fully faded out
    TEST_ASSERT_EQUAL(0, effect.getPwmValue());
}

/**
 * @brief Test a simple mapping rule in the FunctionManager.
 */
void test_manager_mapping_rule() {
    FunctionManager manager;
    manager.addLogicalFunction(new LogicalFunction(new MockEffect()));

    // Rule: F1 ON -> Activate Logical Function 0
    ConditionVariable cv;
    cv.id = 1;
    cv.conditions.push_back({TriggerSource::FUNC_KEY, 1, TriggerComparator::IS_ON});
    manager.addConditionVariable(cv);

    MappingRule rule;
    rule.target_logical_function_id = 0;
    rule.positive_conditions.push_back(1);
    rule.action = MappingAction::TURN_ON;
    manager.addMappingRule(rule);

    // Initial state: F1 is off, effect should be off.
    manager.update(DELTA_MS);
    TEST_ASSERT_FALSE(effect_is_active);

    // Turn F1 on, update, effect should now be on.
    manager.setFunctionState(1, true);
    manager.update(DELTA_MS);
    TEST_ASSERT_TRUE(effect_is_active);
}


// =====================================================================================
// Main Test Runner
// =====================================================================================

void setup() {
    delay(2000); // Required for some boards to connect the serial monitor.
    UNITY_BEGIN();

    // Run all tests
    RUN_TEST(test_effect_steady);
    RUN_TEST(test_effect_dimming);
    RUN_TEST(test_logical_function_activation);
    RUN_TEST(test_effect_strobe);
    RUN_TEST(test_effect_soft_start_stop);
    RUN_TEST(test_manager_mapping_rule);

    UNITY_END();
}

void loop() {
    // Do nothing.
}
