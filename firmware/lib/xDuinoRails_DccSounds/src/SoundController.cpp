#include "SoundController.h"

SoundController::SoundController(uint8_t rx_pin, uint8_t tx_pin) :
    _dfplayer_serial(rx_pin, tx_pin),
    _dfplayer(_dfplayer_serial) {
}

void SoundController::begin() {
    _dfplayer_serial.begin(9600);
    _dfplayer.begin();
}

void SoundController::loop() {
    _dfplayer.loop();
}

void SoundController::play(uint16_t track) {
    _dfplayer.playMp3FolderTrack(track);
}

void SoundController::setVolume(uint8_t volume) {
    _dfplayer.setVolume(volume);
}
