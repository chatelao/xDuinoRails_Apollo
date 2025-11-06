#ifndef CONFIG_H
#define CONFIG_H

// =====================================================================================
// Protocol Configuration
// =====================================================================================
// Select the protocol to use by commenting or uncommenting one of the following
// lines. Only one protocol can be active at a time.
//
// For PlatformIO users: This setting is automatically overridden by the
// environment in `platformio.ini`.
//
// For Arduino IDE users: Make sure that the required libraries
// (e.g. NmraDcc, MaerklinMotorola) are manually installed via the
// Library Manager.

#define PROTOCOL_MM
// #define PROTOCOL_DCC


// =====================================================================================
// Hardware Pin Configuration
// =====================================================================================
// Define the pins used for controlling the motor H-bridge.

// Pin for the PWM signal (speed)
#define MOTOR_PIN_A 0

// Pin for direction switching
#define MOTOR_PIN_B 1

// Pins for BEMF measurement (analog inputs)
#define MOTOR_BEMF_A_PIN A3
#define MOTOR_BEMF_B_PIN A2

// =====================================================================================
// Function Output Configuration (Light & AUX)
// =====================================================================================
// This new system replaces the old, static light configuration.
// It allows for flexible assignment of functions (e.g. light, smoke)
// to outputs and their control via function keys.

// Step 1: Define the physical output pins you want to use.
#define FUNC_PHYSICAL_PIN_0 26 // e.g. front light
#define FUNC_PHYSICAL_PIN_1 27 // e.g. rear light
// #define FUNC_PHYSICAL_PIN_2 28 // e.g. cab light

// Step 2: Configure the "Logical Functions".
// Each logical function combines a physical pin with a behavior (effect)
// and maps it to a function key and optionally to the direction of travel.

// --- Logical Function 0 (e.g. Front Light) ---
#define LOGICAL_FUNC_0_TYPE         STEADY // Effect type (STEADY or DIMMING)
#define LOGICAL_FUNC_0_OUTPUT_PIN   FUNC_PHYSICAL_PIN_0 // Which pin is controlled?
#define LOGICAL_FUNC_0_MAPPED_KEY   0      // Which function key controls the function? (F0)
#define LOGICAL_FUNC_0_DIRECTION    1      // Direction dependency: 1=Forward, -1=Reverse, 0=Always on
#define LOGICAL_FUNC_0_PARAM_1      255    // Parameter 1 (e.g. brightness for STEADY)

// --- Logical Function 1 (e.g. Rear Light) ---
#define LOGICAL_FUNC_1_TYPE         STEADY
#define LOGICAL_FUNC_1_OUTPUT_PIN   FUNC_PHYSICAL_PIN_1
#define LOGICAL_FUNC_1_MAPPED_KEY   0
#define LOGICAL_FUNC_1_DIRECTION    -1
#define LOGICAL_FUNC_1_PARAM_1      255

// To add another function, simply copy a block and
// increase the number (e.g. LOGICAL_FUNC_2_...).


// =====================================================================================
// Motor Configuration for xDuinoRails_MotorControl
// =====================================================================================
// Here the parameters for the extended motor control are defined. Adjust
// these values to optimize the driving behavior of your locomotive.

// Acceleration rate in "pulses per second per second" (PPS^2).
// A higher value means faster acceleration.
// A value of 50 means that the speed increases by 50 PPS every second.
#define MOTOR_ACCELERATION 50

// Braking rate in "pulses per second per second" (PPS^2).
// A higher value means stronger braking.
// A value of 100 means that the speed decreases by 100 PPS every second.
#define MOTOR_DECELERATION 100

// "Startup Kick" to overcome the initial inertia of the motor.
// MOTOR_STARTUP_KICK_PWM: The PWM value (0-255) that is briefly applied.
// MOTOR_STARTUP_KICK_DURATION: The duration of the kick in milliseconds.
#define MOTOR_STARTUP_KICK_PWM 80
#define MOTOR_STARTUP_KICK_DURATION 10


#endif // CONFIG_H
