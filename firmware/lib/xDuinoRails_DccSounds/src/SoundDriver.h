#ifndef SOUNDDRIVER_H
#define SOUNDDRIVER_H

#include <cstdint>
#include <cstddef>

class SoundDriver {
public:
    virtual ~SoundDriver() {}
    virtual bool begin() = 0;
    virtual void play(uint16_t track) = 0;
    virtual void setVolume(uint8_t volume) = 0;
    virtual void loop() = 0;
    virtual void playRaw(const int16_t* pcm_data, size_t data_len) {}
    virtual bool supportsPolyphony() const { return false; }
};

#endif // SOUNDDRIVER_H
