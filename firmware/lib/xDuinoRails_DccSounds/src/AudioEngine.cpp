/**
 * @file AudioEngine.cpp
 * @brief Manages polyphonic audio playback and mixing.
 */

#include "AudioEngine.h"
#include <climits>

AudioEngine::AudioEngine() :
    _driver(nullptr),
    _volume(1.0f) {
}

AudioEngine::~AudioEngine() {
}

void AudioEngine::begin(SoundDriver* driver) {
    _driver = driver;
}

void AudioEngine::play(const WavDecoder* wav, bool loop) {
    if (!wav) return;

    for (int i = 0; i < NUM_SOUND_SLOTS; ++i) {
        if (!_slots[i].isPlaying()) {
            _slots[i].play(wav, loop);
            return;
        }
    }
}

void AudioEngine::loop() {
    mix_and_play();
}

void AudioEngine::mix_and_play() {
    if (!_driver) return;

    for (int i = 0; i < AUDIO_BUFFER_SIZE; ++i) {
        int32_t mixed_sample = 0;

        for (int j = 0; j < NUM_SOUND_SLOTS; ++j) {
            if (_slots[j].isPlaying()) {
                mixed_sample += _slots[j].getNextSample();
            }
        }

        mixed_sample = (int32_t)(mixed_sample * _volume);

        if (mixed_sample > SHRT_MAX) {
            mixed_sample = SHRT_MAX;
        } else if (mixed_sample < SHRT_MIN) {
            mixed_sample = SHRT_MIN;
        }

        _mix_buffer[i] = (int16_t)mixed_sample;
    }

    _driver->playRaw(_mix_buffer, sizeof(_mix_buffer));
}
