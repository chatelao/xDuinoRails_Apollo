#include "LightEffect.h"
#include <math.h>

/**
 * @file LightEffect.cpp
 * @brief Implementation for Phase 1 light effects.
 */

// --- EffectSteady ---

void EffectSteady::update(uint32_t delta_ms) {
    // Nothing to do for a steady light.
}

uint8_t EffectSteady::getPwmValue() {
    return _is_active ? _brightness : 0;
}

// --- EffectDimming ---

EffectDimming::EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed)
    : _brightness_full(brightness_full), _brightness_dimmed(brightness_dimmed) {}

void EffectDimming::update(uint32_t delta_ms) {
    // Nothing to do for a simple dimming effect.
}

uint8_t EffectDimming::getPwmValue() {
    if (!_is_active) {
        return 0;
    }
    return _is_dimmed ? _brightness_dimmed : _brightness_full;
}

void EffectDimming::setDimmed(bool dimmed) {
    _is_dimmed = dimmed;
}

// --- EffectFlicker ---

EffectFlicker::EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed)
    : _base_brightness(base_brightness), _flicker_depth(flicker_depth), _flicker_speed(flicker_speed),
      _noise_position(random(0, 1000)) {
    // flicker_speed is 0-255, map to a reasonable increment range.
    _noise_increment = 0.01f + (flicker_speed / 255.0f) * 0.1f;
}

void EffectFlicker::update(uint32_t delta_ms) {
    if (!_is_active) return;
    // Advance the noise position. The delta_ms ensures speed is frame-rate independent.
    _noise_position += _noise_increment * (delta_ms / 16.67f); // Assuming ~60fps baseline
}

uint8_t EffectFlicker::getPwmValue() {
    if (!_is_active) return 0;

    // Using a pseudo-random noise function if available, otherwise fallback to random()
    // Note: A proper Perlin/Simplex noise function would be ideal here.
    // We simulate it by creating a smoothly varying value.
    float noise_val = (sin(_noise_position) + 1.0f) / 2.0f; // Simple sine wave for smooth noise

    int flicker_amount = (int)(noise_val * _flicker_depth);
    int val = _base_brightness - (_flicker_depth / 2) + flicker_amount;

    return max(0, min(255, val));
}


// --- EffectStrobe ---

EffectStrobe::EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness)
    : _brightness(brightness), _timer(0) {
    if (strobe_frequency_hz == 0) strobe_frequency_hz = 1;
    _strobe_period_ms = 1000 / strobe_frequency_hz;
    _on_time_ms = (_strobe_period_ms * constrain(duty_cycle_percent, 0, 100)) / 100;
}

void EffectStrobe::setActive(bool active) {
    LightEffect::setActive(active);
    if (!active) {
        _timer = 0; // Reset timer when deactivated
    }
}

void EffectStrobe::update(uint32_t delta_ms) {
    if (!_is_active) return;
    _timer = (_timer + delta_ms) % _strobe_period_ms;
}

uint8_t EffectStrobe::getPwmValue() {
    if (!_is_active) return 0;
    return (_timer < _on_time_ms) ? _brightness : 0;
}


// --- EffectMarsLight ---

EffectMarsLight::EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent)
    : _peak_brightness(peak_brightness), _angle(0.0f) {
    if (oscillation_frequency_mhz == 0) oscillation_frequency_mhz = 1;
    _oscillation_period_ms = 1000.0f / (oscillation_frequency_mhz / 1000.0f); // convert mHz to Hz
    _phase_shift_rad = 2.0f * PI * (phase_shift_percent / 100.0f);
    _angle = _phase_shift_rad;
}

void EffectMarsLight::update(uint32_t delta_ms) {
    if (!_is_active) return;
    float increment = (2.0f * PI / _oscillation_period_ms) * delta_ms;
    _angle += increment;
    if (_angle > (2.0f * PI + _phase_shift_rad)) {
        _angle -= 2.0f * PI;
    }
}

uint8_t EffectMarsLight::getPwmValue() {
    if (!_is_active) return 0;
    // sin() returns -1 to 1. We want 0 to 1 for brightness.
    float sin_val = (sin(_angle) + 1.0f) / 2.0f;
    return (uint8_t)(sin_val * _peak_brightness);
}

// --- EffectSoftStartStop ---

EffectSoftStartStop::EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness)
    : _target_brightness(target_brightness), _current_brightness(0.0f) {
    _fade_in_increment = (fade_in_time_ms > 0) ? (float)_target_brightness / fade_in_time_ms : _target_brightness;
    _fade_out_increment = (fade_out_time_ms > 0) ? (float)_target_brightness / fade_out_time_ms : _target_brightness;
}

void EffectSoftStartStop::setActive(bool active) {
    LightEffect::setActive(active);
}

void EffectSoftStartStop::update(uint32_t delta_ms) {
    if (_is_active) {
        // Fade in
        if (_current_brightness < _target_brightness) {
            _current_brightness += _fade_in_increment * delta_ms;
            if (_current_brightness > _target_brightness) {
                _current_brightness = _target_brightness;
            }
        }
    } else {
        // Fade out
        if (_current_brightness > 0) {
            _current_brightness -= _fade_out_increment * delta_ms;
            if (_current_brightness < 0) {
                _current_brightness = 0;
            }
        }
    }
}

uint8_t EffectSoftStartStop::getPwmValue() {
    return (uint8_t)_current_brightness;
}
