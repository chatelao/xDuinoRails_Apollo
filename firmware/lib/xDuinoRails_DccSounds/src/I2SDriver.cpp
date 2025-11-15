#include "I2SDriver.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "generated/i2s.pio.h"
#include "generated/beep_sound.h"

// --- Global State ---
static int dma_channel;

I2SDriver::I2SDriver() {
}

bool I2SDriver::begin() {
    setupPIO();
    setupDMA();
    return true;
}

void I2SDriver::loop() {
}

void I2SDriver::play(uint16_t track) {
    if (track == 1) {
        playRaw((const int16_t*)beep_sound, beep_sound_len);
    }
}

void I2SDriver::setVolume(uint8_t volume) {
    // To be implemented.
}

void I2SDriver::playRaw(const int16_t* pcm_data, size_t data_len) {
    if (dma_channel_is_busy(dma_channel)) {
        return; // Don't interrupt playback
    }

    dma_channel_set_read_addr(dma_channel, pcm_data, false);
    dma_channel_set_trans_count(dma_channel, data_len / sizeof(int16_t), true);
}

void I2SDriver::setupPIO() {
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &i2s_program);

    i2s_program_init(pio, sm, offset, I2S_DIN_PIN, I2S_BCLK_PIN);

    // Calculate clock divider for the desired sample rate
    float div = (float)clock_get_hz(clk_sys) / (SAMPLE_RATE * BITS_PER_SAMPLE * 2);
    pio_sm_set_clkdiv(pio, sm, div);

    pio_sm_set_enabled(pio, sm, true);
}

void I2SDriver::setupDMA() {
    dma_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_channel);

    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio0, 0, true));

    dma_channel_configure(
        dma_channel,
        &c,
        &pio0->txf[0], // Write to PIO0 TX FIFO
        nullptr,      // Read from a buffer set later
        0,            // Transfer count set later
        false         // Don't start immediately
    );
}
