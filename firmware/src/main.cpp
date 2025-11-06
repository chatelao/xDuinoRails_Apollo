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
// Zustand der Beleuchtung (F0)
bool f0_state = true; // Lichter sind beim Start an

// Funktion zur Steuerung der Lichter
void updateLights() {
  if (!f0_state) {
    // Lichter sind ausgeschaltet
    analogWrite(LIGHT_PIN_FWD, 0);
    analogWrite(LIGHT_PIN_REV, 0);
    return;
  }

  if (motor.getDirection()) { // true == vorwärts
    analogWrite(LIGHT_PIN_FWD, LIGHT_BRIGHTNESS);
    analogWrite(LIGHT_PIN_REV, 0);
  } else { // false == rückwärts
    analogWrite(LIGHT_PIN_FWD, 0);
    analogWrite(LIGHT_PIN_REV, LIGHT_BRIGHTNESS);
  }
}


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
    updateLights(); // Lichtstatus nach Richtungsänderung aktualisieren
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

/**
 * @brief Callback function for DCC CV programming.
 *
 * This function is called by the NmraDcc library whenever a CV write
 * is acknowledged. It allows the firmware to react to changes in CV values,
 * for example by updating motor control parameters.
 *
 * @param CV The CV number that was written.
 * @param Value The new value of the CV.
 */
void notifyCVChange(uint16_t CV, uint8_t Value) {
    switch (CV) {
        case CV_START_VOLTAGE:
            // Update motor startup kick PWM value. Duration remains the same.
            motor.setStartupKick(Value, MOTOR_STARTUP_KICK_DURATION);
            break;
        case CV_ACCELERATION_RATE:
            // The CV value is a multiplier; convert it to a suitable PPS^2 value.
            // This conversion factor (e.g., 2.5) may need tuning.
            motor.setAcceleration(Value * 2.5);
            break;
        case CV_DECELERATION_RATE:
            // The CV value is a multiplier; convert it to a suitable PPS^2 value.
            motor.setDeceleration(Value * 2.5);
            break;
        default:
            // Do nothing for other CVs
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
  // Initialize motor controller and load settings from config.h
  pinMode(LIGHT_PIN_FWD, OUTPUT);
  pinMode(LIGHT_PIN_REV, OUTPUT);

  motor.begin();

  updateLights(); // Initiale Licht-Einstellung

#if defined(PROTOCOL_DCC)
  pinMode(HEADLIGHT_PIN, OUTPUT);
  // Configure the DCC library
  dcc.pin(DCC_SIGNAL_PIN, false); // Set the DCC signal pin, not inverted
  dcc.init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0); // Initialize with a DIY manufacturer ID
  dcc.setCVWriteCallback(notifyCVChange); // Register the callback for CV changes

  // Set default CV values
  dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, 3);
  dcc.setCV(CV_START_VOLTAGE, 80);
  dcc.setCV(CV_ACCELERATION_RATE, 20);
  dcc.setCV(CV_DECELERATION_RATE, 40);
  dcc.setCV(CV_MANUFACTURER_ID, 165); // NMRA ID for DIY projects

  // Load motor settings from CVs
  notifyCVChange(CV_START_VOLTAGE, dcc.getCV(CV_START_VOLTAGE));
  notifyCVChange(CV_ACCELERATION_RATE, dcc.getCV(CV_ACCELERATION_RATE));
  notifyCVChange(CV_DECELERATION_RATE, dcc.getCV(CV_DECELERATION_RATE));
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
    // Lichtstatus auswerten
    f0_state = data->Function;
    updateLights();

    if (data->ChangeDir) {
      // Direction change command
      motor.setDirection(!motor.getDirection());
      updateLights(); // Licht nach Richtungswechsel umschalten
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
