/**
 * @file main.cpp
 * @brief Main firmware for the multi-protocol locomotive decoder.
 *
 * This file contains the primary logic for the decoder, including initialization,
 * protocol handling (DCC and Märklin-Motorola), and motor control. It uses
 * conditional compilation to build the firmware for a specific protocol based on
 * defines in `config.h` or `platformio.ini`.
 */
#include <Arduino.h>
#include "config.h"
#include <XDuinoRails_MotorDriver.h>
#include "FunctionManager.h"
#include "PhysicalOutput.h"
#include "LogicalFunction.h"
#include "LightEffect.h"

/**
 * @brief Global motor driver object.
 */
XDuinoRails_MotorDriver motor(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_BEMF_A_PIN, MOTOR_BEMF_B_PIN);

/**
 * @brief Global function manager object.
 */
FunctionManager functionManager;


#if defined(PROTOCOL_DCC)
#define NMRA_DCC_PROCESS_MULTIFUNCTION
#include <NmraDcc.h>

#define DCC_SIGNAL_PIN 7

/**
 * @brief Global NmraDcc object for handling the DCC protocol.
 */
NmraDcc dcc;

/**
 * @brief Callback function for DCC speed commands.
 *
 * This function is called by the NmraDcc library whenever a speed packet
 * matching the decoder's address is received. It updates the motor's
 * direction and target speed.
 *
 * @param Addr The locomotive address from the packet.
 * @param AddrType The address type (primary or extended).
 * @param Speed The raw speed value (0-255).
 * @param Dir The direction (DCC_DIR_FWD or DCC_DIR_REV).
 * @param SpeedSteps The speed step mode (e.g., 128, 28).
 */
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
  if (Addr == dcc.getAddr()) {
    motor.setDirection(Dir == DCC_DIR_FWD);
    // Map DCC speed steps (0-255) to motor's internal PPS (Pulses Per Second)
    int pps = map(Speed, 0, 255, 0, 200);
    motor.setTargetSpeed(pps);
  }
}

/**
 * @brief Helper to process a function group state from the DCC callback.
 * @param start_fn The starting function number for this group (e.g., 5 for FN_5_8).
 * @param count The number of functions in this group (e.g., 4 for FN_5_8).
 * @param state_mask The bitmask of function states from the callback.
 */
void processFunctionGroup(int start_fn, int count, uint8_t state_mask) {
    for (int i = 0; i < count; i++) {
        bool state = (state_mask >> i) & 0x01;
        functionManager.setFunctionState(start_fn + i, state);
    }
}

/**
 * @brief Callback for DCC function key presses, corrected for NmraDcc library.
 *
 * This function is called by the NmraDcc library for each function group packet.
 * It decodes the group and the state of each function within it, then updates
 * the FunctionManager.
 */
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
    if (Addr != dcc.getAddr()) {
        return;
    }

    switch (FuncGrp) {
        case FN_0_4:
            // F0 is bit 4, F1-F4 are bits 0-3
            functionManager.setFunctionState(0, (FuncState & FN_BIT_00) != 0);
            processFunctionGroup(1, 4, FuncState);
            break;
        case FN_5_8:
            processFunctionGroup(5, 4, FuncState);
            break;
        case FN_9_12:
            processFunctionGroup(9, 4, FuncState);
            break;
        case FN_13_20:
            processFunctionGroup(13, 8, FuncState);
            break;
        case FN_21_28:
            processFunctionGroup(21, 8, FuncState);
            break;
        default:
            break;
    }
}

// =====================================================================================
// Protocol-Specific Logic: Märklin-Motorola (MM)
// =====================================================================================
#elif defined(PROTOCOL_MM)
#include <MaerklinMotorola.h>

#define MM_ADDRESS 5
#define MM_SIGNAL_PIN 7

/**
 * @brief Global MaerklinMotorola object for handling the MM protocol.
 */
MaerklinMotorola MM(MM_SIGNAL_PIN);

/**
 * @brief Interrupt Service Routine (ISR) for the MM signal pin.
 *
 * This function is attached to an interrupt on the MM signal pin and must be
 * called on every pin change (RISING or FALLING edge) to allow the library
 * to measure the signal timings.
 */
void mm_isr() {
  MM.PinChange();
}
#endif

// =====================================================================================
// Main Setup and Loop
// =====================================================================================

/**
 * @brief Global setup function, runs once on power-on or reset.
 *
 * Initializes the motor driver and sets up the protocol-specific hardware
 * and libraries.
 */
void setup() {
  // --- Motor-Setup ---
  motor.begin();
  motor.setAcceleration(MOTOR_ACCELERATION);
  motor.setDeceleration(MOTOR_DECELERATION);
  motor.setStartupKick(MOTOR_STARTUP_KICK_PWM, MOTOR_STARTUP_KICK_DURATION);

  // --- Funktions-Setup ---
  // This is where the configuration from config.h is turned into C++ objects.

  // 1. Create global PhysicalOutput objects to avoid memory leaks.
  static PhysicalOutput out_fwd(PO_HEADLIGHT_FWD);
  static PhysicalOutput out_rev(PO_HEADLIGHT_REV);
  static PhysicalOutput out_cab(PO_CABIN_LIGHT);

  // 2. Create LogicalFunctions and assign effects and outputs.
  // The order they are added to the manager determines their F-key mapping (F0, F1, ...).

  // F0: Headlight (Front) - For Phase 1, this is a simple ON/OFF function.
  // Directional logic will be added in a later phase.
  LogicalFunction* lf_headlight_fwd = new LogicalFunction(new EffectSteady(BRIGHTNESS_FULL));
  lf_headlight_fwd->addOutput(&out_fwd);
  functionManager.addLogicalFunction(lf_headlight_fwd);

  // F1: Headlight (Rear) - For Phase 1, this is also a simple ON/OFF function.
  LogicalFunction* lf_headlight_rev = new LogicalFunction(new EffectSteady(BRIGHTNESS_FULL));
  lf_headlight_rev->addOutput(&out_rev);
  functionManager.addLogicalFunction(lf_headlight_rev);

  // F2: Cabin Light
  LogicalFunction* lf_cabin_light = new LogicalFunction(new EffectSteady(BRIGHTNESS_DIMMED));
  lf_cabin_light->addOutput(&out_cab);
  functionManager.addLogicalFunction(lf_cabin_light);

  // --- Protokoll-Setup ---
#if defined(PROTOCOL_DCC)
  // Configure the DCC library
  dcc.pin(DCC_SIGNAL_PIN, false); // Set the DCC signal pin, not inverted
  dcc.init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0); // Initialize with a DIY manufacturer ID
  dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, 3); // Set the default DCC address to 3
#elif defined(PROTOCOL_MM)
  // Attach the interrupt for the MM signal pin
  attachInterrupt(digitalPinToInterrupt(MM_SIGNAL_PIN), mm_isr, CHANGE);
#endif
}

/**
 * @brief Main loop, runs continuously after setup().
 */
void loop() {
  // Keep track of time for effects
  static uint32_t last_millis = 0;
  uint32_t current_millis = millis();
  uint32_t delta_ms = current_millis - last_millis;
  last_millis = current_millis;

#if defined(PROTOCOL_DCC)
  // Process incoming DCC packets
  dcc.process();
#elif defined(PROTOCOL_MM)
  // Parse the MM signal
  MM.Parse();
  MaerklinMotorolaData* data = MM.GetData();
  if (data && !data->IsMagnet && data->Address == MM_ADDRESS) {
    // Update function state (F0 only for MM)
    functionManager.setFunctionState(0, data->Function);

    if (data->ChangeDir) {
      motor.setDirection(!motor.getDirection());
    } else if (data->Stop) {
      motor.setTargetSpeed(0);
    } else {
      uint8_t pps = map(data->Speed, 0, 14, 0, 200);
      motor.setTargetSpeed(pps);
    }
  }
#endif

  // Update motor controller
  motor.update();

  // Update function manager to process light effects
  functionManager.update(delta_ms);
}
