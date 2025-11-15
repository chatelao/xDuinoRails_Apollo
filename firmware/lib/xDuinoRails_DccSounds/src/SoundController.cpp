#include "SoundController.h"
#include "config.h"
#include "generated/beep_wav.h"
#include "generated/noise_wav.h"

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
    if (_driver && _driver->begin()) {
        if (_driver->supportsPolyphony()) {
            _audioEngine.begin(_driver);
            _sounds[0].load(beep_wav, beep_wav_len);
            _sounds[1].load(noise_wav, noise_wav_len);
        }
        return true;
    }
    return false;
}

void SoundController::play(uint16_t track) {
    if (!_driver) return;

    if (_driver->supportsPolyphony()) {
        if (track > 0 && track <= MAX_SOUNDS) {
            _audioEngine.play(&_sounds[track - 1]);
        }
    } else {
        // For non-polyphonic drivers, pass the track number directly
        _driver->play(track);
    }
}

void SoundController::setVolume(uint8_t volume) {
    if (!_driver) return;

    if (_driver->supportsPolyphony()) {
        _audioEngine.setVolume(volume / 255.0f);
    } else {
        _driver->setVolume(volume);
    }
}

void SoundController::loop() {
    if (!_driver) return;

    if (_driver->supportsPolyphony()) {
        _audioEngine.loop();
    } else {
        _driver->loop();
    }
}
