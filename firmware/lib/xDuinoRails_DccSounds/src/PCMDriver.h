#ifndef PCMDRIVER_H
#define PCMDRIVER_H

#include "SoundDriver.h"

class PCMDriver : public SoundDriver {
public:
    PCMDriver(uint8_t pcm_pin);

    bool begin() override;
    void play(uint16_t track) override;
    void setVolume(uint8_t volume) override;
    void loop() override;

private:
    uint8_t _pcm_pin;
};

#endif // PCMDRIVER_H
