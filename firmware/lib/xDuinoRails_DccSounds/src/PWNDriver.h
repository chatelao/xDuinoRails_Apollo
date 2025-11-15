#ifndef PWMDRIVER_H
#define PWMDRIVER_H

#include "SoundDriver.h"

class PWNDriver : public SoundDriver {
public:
    PWNDriver(uint8_t pwm_pin);

    bool begin() override;
    void play(uint16_t track) override;
    void setVolume(uint8_t volume) override;
    void loop() override;

private:
    uint8_t _pwm_pin;
};

#endif // PWMDRIVER_H
