/**
 * @file SoundSlot.cpp
 * @brief Represents a single audio channel for polyphony.
 */

#include "SoundSlot.h"

SoundSlot::SoundSlot() :
    _currentSound(nullptr),
    _currentSample(0),
    _isPlaying(false),
    _loop(false) {
}

SoundSlot::~SoundSlot() {
    // Destructor implementation
}

void SoundSlot::play(const WavDecoder* wav, bool loop) {
    if (wav) {
        _currentSound = wav;
        _currentSample = 0;
        _isPlaying = true;
        _loop = loop;
    }
}

void SoundSlot::stop() {
    _isPlaying = false;
    _currentSound = nullptr;
}

int16_t SoundSlot::getNextSample() {
    if (!_isPlaying || !_currentSound) {
        return 0;
    }

    const int16_t* pcm_data = _currentSound->getPcmData();
    uint32_t num_samples = _currentSound->getNumSamples();

    if (_currentSample >= num_samples) {
        if (_loop) {
            _currentSample = 0;
        } else {
            stop();
            return 0;
        }
    }

    return pcm_data[_currentSample++];
}
