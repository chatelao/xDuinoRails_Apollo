#include <unity.h>
#include "../mocks/Arduino.h"
#include "FunctionManager.h" // Include the header for the class we are testing

// --- Test Globals ---
FunctionManager* fm;
PhysicalOutput* out1;
PhysicalOutput* out2;
LightEffect* effect1;
LightEffect* effect2;

// --- Mock Arduino Source ---
// We need to include the cpp file here for the linker to find the mock definitions
#include "../mocks/Arduino.cpp"
#include "../../src/FunctionManager.cpp"

void setUp(void) {
    // This is called before each test
    mock.reset();
    fm = new FunctionManager();
    out1 = new PhysicalOutput(26);
    out1->begin();
    out2 = new PhysicalOutput(27);
    out2->begin();
    effect1 = new EffectSteady(255);
    effect2 = new EffectSteady(128);
}

void tearDown(void) {
    // This is called after each test
    delete fm;
    fm = nullptr;
    delete out1;
    delete out2;
    // Effects are deleted by the FunctionManager's destructor chain
}

// Verifies that a registered function is off by default.
void test_initial_state_is_off(void) {
    fm->registerFunction(0, out1, effect1, 1); // F0, FWD
    fm->update();
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[26]);
}

// Verifies that a function can be turned on and off using setFunctionKeyState.
void test_function_toggles_on_and_off(void) {
    fm->registerFunction(1, out1, effect1, 0); // F1, No direction dependency

    // Turn ON
    fm->setFunctionKeyState(1, true);
    fm->update();
    TEST_ASSERT_EQUAL(255, mock.analogWrite_values[26]);

    // Turn OFF
    fm->setFunctionKeyState(1, false);
    fm->update();
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[26]);
}

// Verifies that a function with a forward dependency is only active when the direction is set to forward.
void test_direction_dependency_forward_only(void) {
    fm->registerFunction(0, out1, effect1, 1); // F0, FWD only

    fm->setFunctionKeyState(0, true);

    // Set direction to FORWARD
    fm->setDirection(true);
    fm->update();
    TEST_ASSERT_EQUAL(255, mock.analogWrite_values[26]);


    // Set direction to REVERSE
    fm->setDirection(false);
    fm->update();
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[26]);
}

// Verifies that a function with a reverse dependency is only active when the direction is set to reverse.
void test_direction_dependency_reverse_only(void) {
    fm->registerFunction(0, out1, effect2, -1); // F0, REV only, brightness 128

    fm->setFunctionKeyState(0, true);

    // Set direction to FORWARD
    fm->setDirection(true);
    fm->update();
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[26]);

    // Set direction to REVERSE
    fm->setDirection(false);
    fm->update();
    TEST_ASSERT_EQUAL(128, mock.analogWrite_values[26]);
}

// Verifies that multiple functions can be mapped to the same key and controlled by direction.
void test_multiple_functions_on_same_key(void) {
    // This tests the classic direction-dependent headlight scenario
    fm->registerFunction(0, out1, effect1, 1);  // F0, FWD, pin 26, bright 255
    fm->registerFunction(0, out2, effect2, -1); // F0, REV, pin 27, bright 128

    fm->setFunctionKeyState(0, true);

    // Check FORWARD direction
    fm->setDirection(true);
    fm->update();
    TEST_ASSERT_EQUAL(255, mock.analogWrite_values[26]);
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[27]);
}


// Verifies the full direction-dependent headlight scenario, including switching between forward and reverse.
void test_headlight_scenario(void) {
    // F0 controls front and rear lights based on direction
    fm->registerFunction(0, out1, effect1, 1);  // F0 -> Front Light (Pin 26), FWD
    fm->registerFunction(0, out2, new EffectSteady(200), -1); // F0 -> Rear Light (Pin 27), REV

    fm->setFunctionKeyState(0, true);

    // Go FORWARD
    fm->setDirection(true);
    fm->update();
    TEST_ASSERT_EQUAL(255, mock.analogWrite_values[26]);
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[27]);

    // Go REVERSE
    fm->setDirection(false);
    fm->update();
    TEST_ASSERT_EQUAL(0, mock.analogWrite_values[26]);
    TEST_ASSERT_EQUAL(200, mock.analogWrite_values[27]);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state_is_off);
    RUN_TEST(test_function_toggles_on_and_off);
    RUN_TEST(test_direction_dependency_forward_only);
    RUN_TEST(test_direction_dependency_reverse_only);
    RUN_TEST(test_multiple_functions_on_same_key);
    RUN_TEST(test_headlight_scenario);
    UNITY_END();
    return 0;
}
