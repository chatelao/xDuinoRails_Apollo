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
#include "FunctionManager.cpp"


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
 * @brief Test that the manager correctly maps F-keys to functions.
 */
void test_manager_direct_mapping() {
    FunctionManager manager;
    manager.addLogicalFunction(new LogicalFunction(new MockEffect()));
    manager.setFunctionState(0, true);
    TEST_ASSERT_TRUE(effect_is_active);
    manager.setFunctionState(0, false);
    TEST_ASSERT_FALSE(effect_is_active);
    manager.setFunctionState(5, true);
    TEST_ASSERT_FALSE(effect_is_active);
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
    RUN_TEST(test_manager_direct_mapping);

    UNITY_END();
}

void loop() {
    // Do nothing.
}
