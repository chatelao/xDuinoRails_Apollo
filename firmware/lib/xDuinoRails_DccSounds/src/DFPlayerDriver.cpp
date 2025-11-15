#include "DFPlayerDriver.h"

DFPlayerDriver::DFPlayerDriver(uint8_t rx_pin, uint8_t tx_pin) :
    _dfplayer_serial(rx_pin, tx_pin),
    _dfplayer(_dfplayer_serial)
{
}

bool DFPlayerDriver::begin() {
    _dfplayer_serial.begin(9600);
    // The library doesn't have a begin() method that takes a stream.
    // It's initialized in the constructor.
    return true;
}

void DFPlayerDriver::loop() {
    _dfplayer.loop();
}

void DFPlayerDriver::play(uint16_t track) {
    _dfplayer.playGlobalTrack(track);
}

void DFPlayerDriver::setVolume(uint8_t volume) {
    // The DFPlayer has a volume range of 0-30. We need to map our 0-255 range.
    uint8_t dfp_volume = map(volume, 0, 255, 0, 30);
    _dfplayer.setVolume(dfp_volume);
}
