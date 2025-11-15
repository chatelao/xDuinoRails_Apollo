#ifndef I2SDRIVER_H
#define I2SDRIVER_H

#include "SoundDriver.h"
#include "config.h"

// --- I2S Pin Configuration ---
#ifndef I2S_BCLK_PIN
#error "I2S_BCLK_PIN must be defined!"
#endif
#ifndef I2S_LRCLK_PIN
#error "I2S_LRCLK_PIN must be defined!"
#endif
#ifndef I2S_DIN_PIN
#error "I2S_DIN_PIN must be defined!"
#endif

// --- Audio Format Configuration ---
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16

class I2SDriver : public SoundDriver {
public:
    I2SDriver();

    bool begin() override;
    void play(uint16_t track) override;
    void setVolume(uint8_t volume) override;
    void loop() override;
    void playRaw(const int16_t* pcm_data, size_t data_len) override;
    bool supportsPolyphony() const override { return true; }

private:
    void setupPIO();
    void setupDMA();
};

#endif // I2SDRIVER_H
