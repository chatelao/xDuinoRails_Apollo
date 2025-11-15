/**
 * @file AudioEngine.h
 * @brief Manages polyphonic audio playback and mixing.
 */

#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "SoundSlot.h"
#include "WavDecoder.h"
#include "SoundDriver.h"

#define NUM_SOUND_SLOTS 16
#define AUDIO_BUFFER_SIZE 256

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    void begin(SoundDriver* driver);
    void loop();
    void play(const WavDecoder* wav, bool loop = false);
    void setVolume(float volume) { _volume = volume; }

private:
    void mix_and_play();

    SoundDriver* _driver;
    SoundSlot _slots[NUM_SOUND_SLOTS];
    int16_t _mix_buffer[AUDIO_BUFFER_SIZE];
    float _volume;
};

#endif // AUDIOENGINE_H
