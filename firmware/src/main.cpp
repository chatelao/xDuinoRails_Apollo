#include <Arduino.h>
#include "config.h"
#include <XDuinoRails_MotorDriver.h>

// Globales Motor-Objekt erstellen
XDuinoRails_MotorDriver motor(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_BEMF_A_PIN, MOTOR_BEMF_B_PIN);

#if defined(PROTOCOL_DCC)
#define NMRA_DCC_PROCESS_MULTIFUNCTION
#include <NmraDcc.h>

#define DCC_SIGNAL_PIN 7
#define HEADLIGHT_PIN 3

NmraDcc dcc;

void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
  if (Addr == dcc.getAddr()) {
    motor.setDirection(Dir == DCC_DIR_FWD);
    int pps = map(Speed, 0, 255, 0, 200);
    motor.setTargetSpeed(pps);
  }
}

void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
  if (Addr == dcc.getAddr()) {
    if (FuncGrp == FN_0_4) {
      if (FuncState & FN_BIT_00) {
        digitalWrite(HEADLIGHT_PIN, HIGH);
      } else {
        digitalWrite(HEADLIGHT_PIN, LOW);
      }
    }
  }
}

#elif defined(PROTOCOL_MM)
#include <MaerklinMotorola.h>

#define MM_ADDRESS 5
#define MM_SIGNAL_PIN 7

MaerklinMotorola MM(MM_SIGNAL_PIN);

void mm_isr() {
  MM.PinChange();
}
#endif

void setup() {
  motor.begin();
  motor.setAcceleration(MOTOR_ACCELERATION);
  motor.setDeceleration(MOTOR_DECELERATION);
  motor.setStartupKick(MOTOR_STARTUP_KICK_PWM, MOTOR_STARTUP_KICK_DURATION);

#if defined(PROTOCOL_DCC)
  pinMode(HEADLIGHT_PIN, OUTPUT);
  dcc.pin(DCC_SIGNAL_PIN, false);
  dcc.init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0);
  dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, 3);
#elif defined(PROTOCOL_MM)
  attachInterrupt(digitalPinToInterrupt(MM_SIGNAL_PIN), mm_isr, CHANGE);
#endif
}

void loop() {
#if defined(PROTOCOL_DCC)
  dcc.process();
#elif defined(PROTOCOL_MM)
  MM.Parse();
  MaerklinMotorolaData* data = MM.GetData();
  if (data && !data->IsMagnet && data->Address == MM_ADDRESS) {
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
  motor.update();
}
