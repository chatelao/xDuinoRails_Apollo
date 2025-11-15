#include "PWMDriver.h"
#include "generated/beep_sound.h"
#include "hardware/pwm.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"

// --- Global State ---
static int pwm_dma_channel;
static uint8_t pwm_audio_buffer[sizeof(beep_sound) / 2];

PWMDriver::PWMDriver(uint8_t pwm_pin) : _pwm_pin(pwm_pin) {
}

bool PWMDriver::begin() {
    // --- GPIO ---
    gpio_set_function(_pwm_pin, GPIO_FUNC_PWM);
    int slice_num = pwm_gpio_to_slice_num(_pwm_pin);

    // --- PWM ---
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f); // Run at full speed
    pwm_config_set_wrap(&config, 255);
    pwm_init(slice_num, &config, true);

    // --- DMA ---
    pwm_dma_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(pwm_dma_channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pwm_get_dreq(slice_num));

    dma_channel_configure(
        pwm_dma_channel,
        &c,
        &pwm_hw->slice[slice_num].cc, // Write to PWM slice compare value
        nullptr,      // Read from a buffer set later
        0,            // Transfer count set later
        false         // Don't start immediately
    );

    return true;
}

void PWMDriver::loop() {
}

void PWMDriver::play(uint16_t track) {
    if (track == 1) {
        if (dma_channel_is_busy(pwm_dma_channel)) {
            return;
        }

        // Convert 16-bit to 8-bit
        for (size_t i = 0; i < beep_sound_len / 2; i++) {
            pwm_audio_buffer[i] = ((int16_t*)beep_sound)[i] >> 8;
        }

        dma_channel_set_read_addr(pwm_dma_channel, pwm_audio_buffer, false);
        dma_channel_set_trans_count(pwm_dma_channel, beep_sound_len / 2, true);
    }
}

void PWMDriver::setVolume(uint8_t volume) {
    // To be implemented.
}
