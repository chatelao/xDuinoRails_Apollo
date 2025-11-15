#include "PCMDriver.h"
#include "generated/beep_sound.h"
#include "pico/stdlib.h"

PCMDriver::PCMDriver(uint8_t pcm_pin) : _pcm_pin(pcm_pin) {
}

bool PCMDriver::begin() {
    gpio_init(_pcm_pin);
    gpio_set_dir(_pcm_pin, GPIO_OUT);
    return true;
}

void PCMDriver::loop() {
}

void PCMDriver::play(uint16_t track) {
    if (track == 1) {
        // Simple blocking implementation for now
        for (size_t i = 0; i < beep_sound_len / 2; i++) {
            uint8_t sample = ((int16_t*)beep_sound)[i] >> 8;
            for (int j = 0; j < 8; j++) {
                gpio_put(_pcm_pin, (sample >> j) & 0x01);
                sleep_us(11); // ~8kHz sample rate
            }
        }
    }
}

void PCMDriver::setVolume(uint8_t volume) {
    // Not applicable
}
