#include <Arduino.h>
#include "xDuinoRails_LocoFuncDecoder.h"

// =================================================================================
// Example: Lok Decoder
// Enables Motor Control and Lights. Sound is disabled.
// =================================================================================

LocoFuncDecoder decoder;

#if defined(PROTOCOL_DCC)
#define DCC_SIGNAL_PIN 7
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
    if (globalDecoderInstance) globalDecoderInstance->handleDccSpeed(Addr, Speed, (Dir == DCC_DIR_FWD), SpeedSteps);
}
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
    if (globalDecoderInstance) globalDecoderInstance->handleDccFunc(Addr, FuncGrp, FuncState);
}
void notifyCVChange(uint16_t CV, uint8_t Value) {
    if (globalDecoderInstance) globalDecoderInstance->handleCVChange(CV, Value);
}
#elif defined(PROTOCOL_MM)
#define MM_SIGNAL_PIN 7
void mm_isr() { decoder.getMM().PinChange(); }
#endif

void setup() {
    LocoFuncDecoderConfig config;
    config.enableMotor = true;
    config.enableSound = false; // Disabled
    config.enableLights = true;

    config.motorPinA = 0;
    config.motorPinB = 1;
    config.bemfPinA = A3;
    config.bemfPinB = A2;
    config.useDefaultPinout = true;

    decoder.begin(config);

#if defined(PROTOCOL_DCC)
    decoder.getDcc().pin(DCC_SIGNAL_PIN, false);
    decoder.getDcc().init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0);
    decoder.getDcc().setCVWriteCallback(notifyCVChange);
#elif defined(PROTOCOL_MM)
    attachInterrupt(digitalPinToInterrupt(MM_SIGNAL_PIN), mm_isr, CHANGE);
#endif
}

void loop() {
    decoder.update();
}
