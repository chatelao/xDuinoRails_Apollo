#include <Arduino.h>
#include <NmraDcc.h>

// Define Pin Assignments
#define DCC_PIN 0
#define MOTOR_PIN_1 1
#define MOTOR_PIN_2 2
#define HEADLIGHT_PIN 3

// Create a DCC object
NmraDcc dcc;

// Callback function for DCC speed and direction
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
  if (Addr == dcc.getAddr()) {
    // Control motor based on speed and direction
    if (Dir == DCC_DIR_FWD) {
      analogWrite(MOTOR_PIN_1, Speed);
      digitalWrite(MOTOR_PIN_2, LOW);
    } else {
      digitalWrite(MOTOR_PIN_1, LOW);
      analogWrite(MOTOR_PIN_2, Speed);
    }
  }
}

// Callback function for DCC functions
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

void setup() {
  // Set pin modes
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(HEADLIGHT_PIN, OUTPUT);

  // Initialize DCC library
  dcc.pin(DCC_PIN, false);
  dcc.init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0);

  // Set the decoder address
  dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, 3);
}

void loop() {
  // Process DCC packets
  dcc.process();
}
