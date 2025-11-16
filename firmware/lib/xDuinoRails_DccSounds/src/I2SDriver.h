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

    /**
     * @brief Plays a sound from a raw PCM data buffer.
     * @param pcm_data Pointer to the raw PCM data (16-bit signed).
     * @param data_len Length of the data in bytes.
     */
    void playRaw(const int16_t* pcm_data, size_t data_len);

    size_t availableForWrite() override;
    size_t write(const uint8_t* data, size_t size) override;

private:
    void setupPIO();
    void setupDMA();

    static void dma_handler();

    uint32_t _dma_buffer[256];
    volatile uint16_t _dma_write_ptr;
    volatile uint16_t _dma_read_ptr;
    int _dma_channel_a;
    int _dma_channel_b;
};

#endif // I2SDRIVER_H
