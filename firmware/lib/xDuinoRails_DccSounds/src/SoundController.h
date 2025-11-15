#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H

#include "AudioEngine.h"
#include "WavDecoder.h"
#include "SoundDriver.h"

#define MAX_SOUNDS 2

class SoundController {
public:
    SoundController();
    ~SoundController();

    bool begin();
    void play(uint16_t track);
    void setVolume(uint8_t volume);
    void loop();

private:
    AudioEngine _audioEngine;
    WavDecoder _sounds[MAX_SOUNDS];
    SoundDriver* _driver;
};

#endif // SOUNDCONTROLLER_H
