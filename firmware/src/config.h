/**
 * @file config.h
 * @brief Central configuration file for the locomotive decoder.
 *
 * This file contains all important settings for the decoder's operation,
 * including the selection of the digital protocol (DCC or MM),
 * the assignment of hardware pins for motor control, and the configuration
 * of the motor's driving characteristics.
 *
 * @note For PlatformIO users, some of these settings, especially
 * the protocol selection, are overridden by the environment in the `platformio.ini` file.
 * Arduino IDE users must perform the configuration here manually.
 */

#ifndef CONFIG_H
#define CONFIG_H

// =====================================================================================
// Protocol Configuration
// =====================================================================================
// Select the protocol to be used here by commenting or uncommenting one of the
// following lines. Only one protocol can be active at a time.
//
// For PlatformIO users: This setting is automatically overridden by the
// environment in `platformio.ini`.
//
// For Arduino IDE users: Ensure that the required libraries
// (e.g., NmraDcc, MaerklinMotorola) are manually installed via the
// Library Manager.

/**
 * @brief Defines the digital protocol to be used.
 *
 * - `PROTOCOL_MM`: Activates the Märklin-Motorola protocol.
 * - `PROTOCOL_DCC`: Activates the DCC protocol.
 *
 * @warning Only one protocol may be active at the same time.
 */
#define PROTOCOL_MM
// #define PROTOCOL_DCC


// =====================================================================================
// Hardware Pin Configuration
// =====================================================================================
// Define the pins used for controlling the motor H-bridge here.

/**
 * @brief Pin for the 'A' PWM signal of the motor H-bridge.
 * This pin controls the speed of the motor.
 */
#define MOTOR_PIN_A 0

/**
 * @brief Pin for the 'B' PWM signal of the motor H-bridge.
 * This pin is used together with MOTOR_PIN_A to control the direction of rotation.
 */
#define MOTOR_PIN_B 1

/**
 * @brief Analog input pin for measuring the back-EMF (BEMF) on motor connection A.
 * Used for the advanced load regulation of the motor.
 */
#define MOTOR_BEMF_A_PIN A3

/**
 * @brief Analog input pin for measuring the back-EMF (BEMF) on motor connection B.
 * Used for the advanced load regulation of the motor.
 */
#define MOTOR_BEMF_B_PIN A2

// =====================================================================================
// Function & Output Configuration (Function Mapping)
// =====================================================================================
// All configuration for light and special functions is done in this section.
// The system is structured in three levels:
//
// 1. Physical Outputs:
//    Define all hardware pins that are to be used as outputs here.
//
// 2. Logical Functions:
//    A logical function is a property of the locomotive (e.g., "front headlight").
//    Each logical function is assigned a light effect (e.g., "steady light") and
//    at least one physical output.
//
// 3. Mapping:
//    Here it is defined which key on the command station (e.g., F0) controls which
//    logical function.
//
// NOTE FOR PHASE 1: The mapping is still direct. F0 controls the first
// defined logical function, F1 the second, and so on.
// =====================================================================================

// --- 1. Define Physical Outputs ---
// Each output receives a unique name and a pin number.
#define PO_HEADLIGHT_FWD 26 // Front headlight
#define PO_HEADLIGHT_REV 27 // Rear headlight
#define PO_CABIN_LIGHT 28   // Cabin light (example)
#define PO_SERVO_1 29       // Servo 1 (example)

// --- 2. Define Logical Functions and Effects ---
// Here the setup logic for the FunctionManager is prepared.
// This is done in the `setup()` function in `main.cpp`.
//
// Example configuration:
// - F0: Directional headlight (not yet implemented, for Phase 1 only simple on/off)
// - F1: Cabin light on/off
//
// IMPORTANT: The order of the calls to `functionManager.addLogicalFunction`
// in `main.cpp` determines the mapping:
// - The first call is controlled by F0.
// - The second call is controlled by F1.
// - etc.

// Brightness Definitions
#define BRIGHTNESS_FULL 255
#define BRIGHTNESS_DIMMED 80


// =====================================================================================
// Motor Configuration for xDuinoRails_MotorControl
// =====================================================================================
// The primary motor settings (acceleration, braking time, startup voltage)
// are now configured via DCC CVs (CV 2, 3, 4). The remaining values here
// are for more specific adjustments.

/**
 * @brief Duration of the "Startup Kick" in milliseconds.
 * Defines how long the pulse defined under CV 2 (`MOTOR_STARTUP_KICK_PWM`)
 * is applied. This value is not changeable via CV.
 */
#define MOTOR_STARTUP_KICK_DURATION 10


#endif // CONFIG_H
