/**
 * @file main.cpp
 * @brief Main firmware for the multi-protocol locomotive decoder.
 */
#include <Arduino.h>
#include "config.h"
#include "cv_definitions.h"
#include <XDuinoRails_MotorDriver.h>
#include "CVManager.h"
#include "CVManagerAdapter.h"
#include <xDuinoRails_DccLightsAndFunctions.h>

// --- Global Objects ---
XDuinoRails_MotorDriver motor(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_BEMF_A_PIN, MOTOR_BEMF_B_PIN);
CVManager cvManager;
xDuinoRails::AuxController auxController;
CVManagerAdapter cvManagerAdapter(cvManager);


#if defined(PROTOCOL_DCC)
// --- DCC-Specific Includes and Objects ---
#define NMRA_DCC_PROCESS_MULTIFUNCTION
#include <NmraDcc.h>

#define DCC_SIGNAL_PIN 7

NmraDcc dcc;

// --- DCC Callback Prototypes ---
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps);
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState);
void notifyCVChange(uint16_t CV, uint8_t Value);

#elif defined(PROTOCOL_MM)
// --- MM-Specific Includes and Objects ---
#include <MaerklinMotorola.h>

#define MM_ADDRESS 5
#define MM_SIGNAL_PIN 7

MaerklinMotorola MM(MM_SIGNAL_PIN);

/**
 * @brief Interrupt Service Routine for the Märklin-Motorola signal.
 */
void mm_isr() {
  MM.PinChange();
}
#endif

/**
 * @brief Setup function, called once at startup.
 */
void setup() {
  // --- Initialization ---
  motor.begin();
  cvManager.begin();

  auxController.addPhysicalOutput(PO_HEADLIGHT_FWD, xDuinoRails::OutputType::PWM_LOW_SIDE);
  auxController.addPhysicalOutput(PO_HEADLIGHT_REV, xDuinoRails::OutputType::PWM_LOW_SIDE);
  auxController.addPhysicalOutput(PO_CABIN_LIGHT, xDuinoRails::OutputType::PWM_LOW_SIDE);
  auxController.addPhysicalOutput(PO_SERVO_1, xDuinoRails::OutputType::SERVO);

  // --- Load Configuration ---
  auxController.loadFromCVs(cvManagerAdapter);

  // --- Protocol-Specific Setup ---
#if defined(PROTOCOL_DCC)
  dcc.pin(DCC_SIGNAL_PIN, false);
  dcc.init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0);
  dcc.setCVWriteCallback(notifyCVChange);

  // Sync our CVManager state with the DCC library's internal state
  dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, cvManager.readCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS));
  dcc.setCV(CV_MANUFACTURER_ID, cvManager.readCV(CV_MANUFACTURER_ID));
  dcc.setCV(CV_DECODER_VERSION_ID, cvManager.readCV(CV_DECODER_VERSION_ID));
  dcc.setCV(CV_DECODER_CONFIGURATION, cvManager.readCV(CV_DECODER_CONFIGURATION));

  // Apply motor settings from CVs
  notifyCVChange(CV_START_VOLTAGE, cvManager.readCV(CV_START_VOLTAGE));
  notifyCVChange(CV_ACCELERATION_RATE, cvManager.readCV(CV_ACCELERATION_RATE));
  notifyCVChange(CV_DECELERATION_RATE, cvManager.readCV(CV_DECELERATION_RATE));
#elif defined(PROTOCOL_MM)
  attachInterrupt(digitalPinToInterrupt(MM_SIGNAL_PIN), mm_isr, CHANGE);

  // Apply motor settings from CVs
  motor.setStartupKick(cvManager.readCV(CV_START_VOLTAGE), MOTOR_STARTUP_KICK_DURATION);
  motor.setAcceleration(cvManager.readCV(CV_ACCELERATION_RATE) * 2.5);
  motor.setDeceleration(cvManager.readCV(CV_DECELERATION_RATE) * 2.5);
#endif
}

/**
 * @brief Main loop, called repeatedly.
 */
void loop() {
  static uint32_t last_millis = 0;
  uint32_t current_millis = millis();
  uint32_t delta_ms = current_millis - last_millis;
  last_millis = current_millis;

#if defined(PROTOCOL_DCC)
  dcc.process();
#elif defined(PROTOCOL_MM)
  MM.Parse();
  MaerklinMotorolaData* data = MM.GetData();
  if (data && !data->IsMagnet && data->Address == MM_ADDRESS) {
    auxController.setFunctionState(0, data->Function);

    if (data->ChangeDir) {
      motor.setDirection(!motor.getDirection());
    } else if (data->Stop) {
      motor.setTargetSpeed(0);
    } else {
      uint8_t pps = map(data->Speed, 0, 14, 0, 200);
      motor.setTargetSpeed(pps);
    }
    // Update function manager with MM state
    auxController.setDirection(motor.getDirection() ? xDuinoRails::DECODER_DIRECTION_FORWARD : xDuinoRails::DECODER_DIRECTION_REVERSE);
    auxController.setSpeed(motor.getTargetSpeed());
  }
#endif

  motor.update();
  auxController.update(delta_ms);
}

// --- DCC Callback Implementations ---
#if defined(PROTOCOL_DCC)
/**
 * @brief Callback for DCC speed packets.
 */
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
  if (Addr == dcc.getAddr()) {
    bool is_forward = (Dir == DCC_DIR_FWD);
    motor.setDirection(is_forward);
    int pps = map(Speed, 0, 255, 0, 200);
    motor.setTargetSpeed(pps);

    // Update function manager state
    auxController.setDirection(is_forward ? xDuinoRails::DECODER_DIRECTION_FORWARD : xDuinoRails::DECODER_DIRECTION_REVERSE);
    auxController.setSpeed(pps);
  }
}

/**
 * @brief Helper function to process DCC function groups.
 * @param start_fn The starting function number for this group.
 * @param count The number of functions in this group.
 * @param state_mask A bitmask of the function states.
 */
void processFunctionGroup(int start_fn, int count, uint8_t state_mask) {
    for (int i = 0; i < count; i++) {
        bool state = (state_mask >> i) & 0x01;
        auxController.setFunctionState(start_fn + i, state);
    }
}

/**
 * @brief Callback for DCC function packets.
 */
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
    if (Addr != dcc.getAddr()) return;

    switch (FuncGrp) {
        case FN_0_4:
            auxController.setFunctionState(0, (FuncState & FN_BIT_00) != 0);
            processFunctionGroup(1, 4, FuncState);
            break;
        case FN_5_8:   processFunctionGroup(5, 4, FuncState); break;
        case FN_9_12:  processFunctionGroup(9, 4, FuncState); break;
        case FN_13_20: processFunctionGroup(13, 8, FuncState); break;
        case FN_21_28: processFunctionGroup(21, 8, FuncState); break;
        default: break;
    }
}

/**
 * @brief Callback for DCC CV write packets.
 */
void notifyCVChange(uint16_t CV, uint8_t Value) {
    // 1. Update our master CV store
    cvManager.writeCV(CV, Value);

    // 2. Apply the change to the relevant part of the firmware
    switch (CV) {
        case CV_START_VOLTAGE:
            motor.setStartupKick(Value, MOTOR_STARTUP_KICK_DURATION);
            break;
        case CV_ACCELERATION_RATE:
            motor.setAcceleration(Value * 2.5);
            break;
        case CV_DECELERATION_RATE:
            motor.setDeceleration(Value * 2.5);
            break;
        // NOTE: Other CVs are applied at startup by the CVLoader.
        // A full implementation might re-run the CVLoader here,
        // but that's a complex operation. For now, motor CVs are live-updated.
    }
}
#endif
