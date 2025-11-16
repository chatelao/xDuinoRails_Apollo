#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H

#include "SoundDriver.h"

// --- Sound Driver Selection ---
// The user must define one of these in their `config.h` or build flags.
// #define SOUND_DRIVER_DFPLAYER
// #define SOUND_DRIVER_I2S
// #define SOUND_DRIVER_PWM
// #define SOUND_DRIVER_PCM

class SoundController {
public:
    SoundController();
    ~SoundController();

    bool begin();
    void play(uint16_t track);
    void setVolume(uint8_t volume);
    void loop();
    size_t availableForWrite();
    size_t write(const uint8_t* data, size_t size);

private:
    SoundDriver* _driver;
};

#endif // SOUNDCONTROLLER_H
