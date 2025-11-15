#ifndef DFPLAYERDRIVER_H
#define DFPLAYERDRIVER_H

#include "SoundDriver.h"
#include <SoftwareSerial.h>
#include <DFMiniMp3.h>

class DFPlayerDriver : public SoundDriver {
public:
    DFPlayerDriver(uint8_t rx_pin, uint8_t tx_pin);

    bool begin() override;
    void play(uint16_t track) override;
    void setVolume(uint8_t volume) override;
    void loop() override;

private:
    SoftwareSerial _dfplayer_serial;
    DFMiniMp3<SoftwareSerial> _dfplayer;
};

#endif // DFPLAYERDRIVER_H
