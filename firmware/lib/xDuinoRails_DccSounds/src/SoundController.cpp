#include "SoundController.h"
#include "config.h" // For pin definitions

// --- Include the correct driver header ---
#if defined(SOUND_DRIVER_DFPLAYER)
#include "DFPlayerDriver.h"
#elif defined(SOUND_DRIVER_I2S)
#include "I2SDriver.h"
#elif defined(SOUND_DRIVER_PWM)
#include "PWNDriver.h"
#elif defined(SOUND_DRIVER_PCM)
#include "PCMDriver.h"
#endif

SoundController::SoundController() {
#if defined(SOUND_DRIVER_DFPLAYER)
    _driver = new DFPlayerDriver(DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
#elif defined(SOUND_DRIVER_I2S)
    _driver = new I2SDriver();
#elif defined(SOUND_DRIVER_PWM)
    _driver = new PWNDriver(PWM_SOUND_PIN);
#elif defined(SOUND_DRIVER_PCM)
    _driver = new PCMDriver(PCM_SOUND_PIN);
#else
    _driver = nullptr;
#endif
}

SoundController::~SoundController() {
    if (_driver) {
        delete _driver;
    }
}

bool SoundController::begin() {
    if (_driver) {
        return _driver->begin();
    }
    return false;
}

void SoundController::play(uint16_t track) {
    if (_driver) {
        _driver->play(track);
    }
}

void SoundController::setVolume(uint8_t volume) {
    if (_driver) {
        _driver->setVolume(volume);
    }
}

void SoundController::loop() {
    if (_driver) {
        _driver->loop();
    }
}

size_t SoundController::availableForWrite() {
    if (_driver) {
        return _driver->availableForWrite();
    }
    return 0;
}

size_t SoundController::write(const uint8_t* data, size_t size) {
    if (_driver) {
        return _driver->write(data, size);
    }
    return 0;
}
