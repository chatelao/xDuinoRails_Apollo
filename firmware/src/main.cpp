#include <Arduino.h>
#include "config.h"
#include <XDuinoRails_MotorDriver.h>
#include "FunctionManager.h"

// Globales Motor-Objekt erstellen
XDuinoRails_MotorDriver motor(MOTOR_PIN_A, MOTOR_PIN_B, MOTOR_BEMF_A_PIN, MOTOR_BEMF_B_PIN);

// Globalen FunctionManager und physische Ausgänge erstellen
FunctionManager functionManager;

#ifdef FUNC_PHYSICAL_PIN_0
PhysicalOutput F_OUT_0(FUNC_PHYSICAL_PIN_0);
#endif
#ifdef FUNC_PHYSICAL_PIN_1
PhysicalOutput F_OUT_1(FUNC_PHYSICAL_PIN_1);
#endif
#ifdef FUNC_PHYSICAL_PIN_2
PhysicalOutput F_OUT_2(FUNC_PHYSICAL_PIN_2);
#endif
// Fügen Sie hier bei Bedarf weitere PhysicalOutput-Objekte hinzu


#if defined(PROTOCOL_DCC)
#define NMRA_DCC_PROCESS_MULTIFUNCTION
#include <NmraDcc.h>

#define DCC_SIGNAL_PIN 7

NmraDcc dcc;

void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
  if (Addr == dcc.getAddr()) {
    bool is_forward = (Dir == DCC_DIR_FWD);
    motor.setDirection(is_forward);
    functionManager.setDirection(is_forward);
    int pps = map(Speed, 0, 255, 0, 200);
    motor.setTargetSpeed(pps);
  }
}

void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
  if (Addr == dcc.getAddr()) {
    if (FuncGrp == FN_0_4) {
      functionManager.setFunctionKeyState(0, FuncState & FN_BIT_00);
      functionManager.setFunctionKeyState(1, FuncState & FN_BIT_01);
      functionManager.setFunctionKeyState(2, FuncState & FN_BIT_02);
      functionManager.setFunctionKeyState(3, FuncState & FN_BIT_03);
      functionManager.setFunctionKeyState(4, FuncState & FN_BIT_04);
    }
    // Fügen Sie hier bei Bedarf weitere Funktionsgruppen (FN_5_8, etc.) hinzu
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
  // --- Initialisierung der physischen Ausgänge ---
#ifdef FUNC_PHYSICAL_PIN_0
  F_OUT_0.begin();
#endif
#ifdef FUNC_PHYSICAL_PIN_1
  F_OUT_1.begin();
#endif
#ifdef FUNC_PHYSICAL_PIN_2
  F_OUT_2.begin();
#endif
  // Initialisieren Sie hier weitere Ausgänge...

  // --- Registrierung der logischen Funktionen ---
#ifdef LOGICAL_FUNC_0_TYPE
  functionManager.registerFunction(
    LOGICAL_FUNC_0_MAPPED_KEY,
    &F_OUT_0,
    new EffectSteady(LOGICAL_FUNC_0_PARAM_1),
    LOGICAL_FUNC_0_DIRECTION
  );
#endif
#ifdef LOGICAL_FUNC_1_TYPE
  functionManager.registerFunction(
    LOGICAL_FUNC_1_MAPPED_KEY,
    &F_OUT_1,
    new EffectSteady(LOGICAL_FUNC_1_PARAM_1),
    LOGICAL_FUNC_1_DIRECTION
  );
#endif
  // Fügen Sie hier die Registrierung für weitere logische Funktionen hinzu...

  motor.begin();
  motor.setAcceleration(MOTOR_ACCELERATION);
  motor.setDeceleration(MOTOR_DECELERATION);
  motor.setStartupKick(MOTOR_STARTUP_KICK_PWM, MOTOR_STARTUP_KICK_DURATION);

  // Initialen Zustand der Funktionen setzen
  functionManager.setDirection(motor.getDirection());
  functionManager.setFunctionKeyState(0, true); // F0 ist standardmäßig an

#if defined(PROTOCOL_DCC)
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
    functionManager.setFunctionKeyState(0, data->Function);

    if (data->ChangeDir) {
      bool new_dir = !motor.getDirection();
      motor.setDirection(new_dir);
      functionManager.setDirection(new_dir);
    } else if (data->Stop) {
      motor.setTargetSpeed(0);
    } else {
      uint8_t pps = map(data->Speed, 0, 14, 0, 200);
      motor.setTargetSpeed(pps);
    }
  }
#endif
  motor.update();
  functionManager.update();
}
