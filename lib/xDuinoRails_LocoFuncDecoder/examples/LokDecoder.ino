#include <xDuinoRails_LocoFuncDecoder.h>

xDuinoRails_LocoFuncDecoder decoder;

void setup() {
  decoder.begin();
}

void loop() {
  decoder.loop();
}
