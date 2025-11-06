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

/**
 * @brief Global motor driver object.
 *
 * This object provides an interface to control the locomotive's motor,
 * managing speed, direction, acceleration, and BEMF sensing.
 */
XDuinoRails_MotorDriver motor(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_BEMF_A_PIN, MOTOR_BEMF_B_PIN);

// =====================================================================================
// Protocol-Specific Logic: DCC
// =====================================================================================
#if defined(PROTOCOL_DCC)
#define NMRA_DCC_PROCESS_MULTIFUNCTION
#include <NmraDcc.h>

#define DCC_SIGNAL_PIN 7
#define HEADLIGHT_PIN 3

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
 * @brief Callback function for DCC function commands.
 *
 * This function is called by the NmraDcc library for function packets (e.g., F0-F28).
 * It currently handles F0 to toggle the headlight.
 *
 * @param Addr The locomotive address from the packet.
 * @param AddrType The address type.
 * @param FuncGrp The function group (e.g., FN_0_4).
 * @param FuncState A bitmask representing the state of functions in the group.
 */
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
  if (Addr == dcc.getAddr()) {
    if (FuncGrp == FN_0_4) {
      // Check if Function 0 (F0) is active
      if (FuncState & FN_BIT_00) {
        digitalWrite(HEADLIGHT_PIN, HIGH);
      } else {
        digitalWrite(HEADLIGHT_PIN, LOW);
      }
    }
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
  // Initialize motor controller and load settings from config.h
  motor.begin();
  motor.setAcceleration(MOTOR_ACCELERATION);
  motor.setDeceleration(MOTOR_DECELERATION);
  motor.setStartupKick(MOTOR_STARTUP_KICK_PWM, MOTOR_STARTUP_KICK_DURATION);

#if defined(PROTOCOL_DCC)
  pinMode(HEADLIGHT_PIN, OUTPUT);
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
 *
 * This function is the heart of the firmware. It polls the appropriate
 * digital protocol library for new commands and periodically updates the

 * motor's state.
 */
void loop() {
#if defined(PROTOCOL_DCC)
  // Process incoming DCC packets
  dcc.process();
#elif defined(PROTOCOL_MM)
  // Parse the MM signal based on timings captured by the ISR
  MM.Parse();
  MaerklinMotorolaData* data = MM.GetData();
  // Check if a valid command for our address has been received
  if (data && !data->IsMagnet && data->Address == MM_ADDRESS) {
    if (data->ChangeDir) {
      // Direction change command
      motor.setDirection(!motor.getDirection());
    } else if (data->Stop) {
      // Stop command
      motor.setTargetSpeed(0);
    } else {
      // Speed command
      uint8_t pps = map(data->Speed, 0, 14, 0, 200); // Map MM speed (0-14) to PPS
      motor.setTargetSpeed(pps);
    }
  }
#endif
  // Update the motor controller (manages acceleration, deceleration, etc.)
  motor.update();
}
