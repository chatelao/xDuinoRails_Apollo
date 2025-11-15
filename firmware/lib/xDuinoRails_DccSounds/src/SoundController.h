#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

class SoundController {
public:
    SoundController(uint8_t rx_pin, uint8_t tx_pin);
    void begin();
    void loop();
    void play(uint16_t track);
    void setVolume(uint8_t volume);

private:
    SoftwareSerial _dfplayer_serial;
    DFMiniMp3<SoftwareSerial> _dfplayer;
};

#endif // SOUNDCONTROLLER_H
