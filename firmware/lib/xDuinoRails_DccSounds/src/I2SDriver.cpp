#include "I2SDriver.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "generated/i2s.pio.h"
#include "generated/beep_sound.h"

// --- Global State ---
static I2SDriver* _instance;

I2SDriver::I2SDriver() : _dma_write_ptr(0), _dma_read_ptr(0) {
    _instance = this;
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
    // Simple implementation: just copy the data into the buffer, overwriting whatever is there.
    // This is not ideal for mixing, but it's good enough for simple sound effects.
    size_t samples_to_write = min(data_len / sizeof(int16_t), 256);
    for (size_t i = 0; i < samples_to_write; i++) {
        _dma_buffer[i] = (uint32_t)pcm_data[i] << 16 | (uint32_t)pcm_data[i];
    }
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

void I2SDriver::dma_handler() {
    if (dma_channel_get_irq0_status(_instance->_dma_channel_a)) {
        dma_channel_acknowledge_irq0(_instance->_dma_channel_a);
        _instance->_dma_read_ptr = 128;
        dma_channel_start(_instance->_dma_channel_a);
    } else if (dma_channel_get_irq0_status(_instance->_dma_channel_b)) {
        dma_channel_acknowledge_irq0(_instance->_dma_channel_b);
        _instance->_dma_read_ptr = 0;
        dma_channel_start(_instance->_dma_channel_b);
    }
}

void I2SDriver::setupDMA() {
    _dma_channel_a = dma_claim_unused_channel(true);
    _dma_channel_b = dma_claim_unused_channel(true);

    dma_channel_config c = dma_channel_get_default_config(_dma_channel_a);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio0, 0, true));

    dma_channel_configure(
        _dma_channel_a,
        &c,
        &pio0->txf[0],
        _dma_buffer,
        256 / 2,
        false
    );

    c = dma_channel_get_default_config(_dma_channel_b);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio0, 0, true));

    dma_channel_configure(
        _dma_channel_b,
        &c,
        &pio0->txf[0],
        &_dma_buffer[256 / 2],
        256 / 2,
        false
    );

    dma_channel_set_irq0_enabled(_dma_channel_a, true);
    dma_channel_set_irq0_enabled(_dma_channel_b, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_channel_start(_dma_channel_a);
}

size_t I2SDriver::availableForWrite() {
    int16_t diff = _dma_read_ptr - _dma_write_ptr;
    if (diff <= 0) {
        diff += 256;
    }
    return (diff - 1) * sizeof(uint32_t);
}

size_t I2SDriver::write(const uint8_t* data, size_t size) {
    size_t bytes_to_write = min(size, availableForWrite());
    size_t samples_to_write = bytes_to_write / sizeof(uint32_t);

    for (size_t i = 0; i < samples_to_write; i++) {
        _dma_buffer[_dma_write_ptr] = ((uint32_t*)data)[i];
        _dma_write_ptr = (_dma_write_ptr + 1) % 256;
    }

    return samples_to_write * sizeof(uint32_t);
}
