/**
 * @file SoundSlot.h
 * @brief Represents a single audio channel for polyphony.
 */

#ifndef SOUNDSLOT_H
#define SOUNDSLOT_H

#include <cstdint>
#include "WavDecoder.h"

class SoundSlot {
public:
    SoundSlot();
    ~SoundSlot();

    void play(const WavDecoder* wav, bool loop = false);
    void stop();
    bool isPlaying() const { return _isPlaying; }
    int16_t getNextSample();

private:
    const WavDecoder* _currentSound;
    uint32_t _currentSample;
    bool _isPlaying;
    bool _loop;
};

#endif // SOUNDSLOT_H
