#include "Effect.h"
#include <math.h>

/**
 * @file Effect.cpp
 * @brief Implementation for all logical effects.
 */

// --- EffectSteady ---

void EffectSteady::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    uint8_t value = _is_active ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// --- EffectServo ---

EffectServo::EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed)
    : _endpoint_a(endpoint_a), _endpoint_b(endpoint_b), _current_angle(endpoint_a), _target_angle(endpoint_a) {
    // travel_speed is 0-255, map to a reasonable degrees per ms range.
    // A speed of 0 should be instant.
    if (travel_speed == 0) {
        _speed = 180.0f; // effectively instant
    } else {
        // Map 1-255 to a range like 0.01 deg/ms to 0.5 deg/ms
        _speed = 0.01f + (travel_speed / 255.0f) * 0.49f;
    }
}

void EffectServo::setActive(bool active) {
    if (active && !_is_active) {
        // Latching behavior: toggle target endpoint on activation.
        _target_angle = _is_at_a ? _endpoint_b : _endpoint_a;
        _is_at_a = !_is_at_a;
    }
    Effect::setActive(active);
    // Note: Servo stays active to hold position. Deactivation logic is handled in update.
}

void EffectServo::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (_current_angle != _target_angle) {
        float delta_angle = _speed * delta_ms;
        if (_current_angle < _target_angle) {
            _current_angle += delta_angle;
            if (_current_angle > _target_angle) {
                _current_angle = _target_angle;
            }
        } else {
            _current_angle -= delta_angle;
            if (_current_angle < _target_angle) {
                _current_angle = _target_angle;
            }
        }
    }

    // Deactivate after the move is complete (for momentary mode in the future)
    // For now, with latching, we might want it to stay active to provide holding torque.
    // If we were to implement momentary, it would be something like:
    // if (!_is_active && _current_angle == _endpoint_a) { /* truly finished */ }

    for (auto* output : outputs) {
        output->setServoAngle((uint16_t)_current_angle);
    }
}

// --- EffectSmokeGenerator ---

EffectSmokeGenerator::EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed)
    : _heater_enabled(heater_enabled), _fan_speed(fan_speed) {}

void EffectSmokeGenerator::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (outputs.empty()) {
        return;
    }

    uint8_t heater_value = (_is_active && _heater_enabled) ? 255 : 0;
    uint8_t fan_value = _is_active ? _fan_speed : 0;

    // Convention: First output is the heater, second is the fan.
    if (outputs.size() > 0) {
        outputs[0]->setValue(heater_value);
    }
    if (outputs.size() > 1) {
        outputs[1]->setValue(fan_value);
    }
}

// --- EffectDimming ---

EffectDimming::EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed)
    : _brightness_full(brightness_full), _brightness_dimmed(brightness_dimmed) {}

void EffectDimming::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    uint8_t value = 0;
    if (_is_active) {
        value = _is_dimmed ? _brightness_dimmed : _brightness_full;
    }
    for (auto* output : outputs) {
        output->setValue(value);
    }
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

void EffectFlicker::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) {
            output->setValue(0);
        }
        return;
    }

    _noise_position += _noise_increment * (delta_ms / 16.67f); // Assuming ~60fps baseline

    float noise_val = (sin(_noise_position) + 1.0f) / 2.0f; // Simple sine wave for smooth noise
    int flicker_amount = (int)(noise_val * _flicker_depth);
    int val = _base_brightness - (_flicker_depth / 2) + flicker_amount;
    uint8_t value = max(0, min(255, val));

    for (auto* output : outputs) {
        output->setValue(value);
    }
}


// --- EffectStrobe ---

EffectStrobe::EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness)
    : _brightness(brightness), _timer(0) {
    if (strobe_frequency_hz == 0) strobe_frequency_hz = 1;
    _strobe_period_ms = 1000 / strobe_frequency_hz;
    _on_time_ms = (_strobe_period_ms * constrain(duty_cycle_percent, 0, 100)) / 100;
}

void EffectStrobe::setActive(bool active) {
    Effect::setActive(active);
    if (!active) {
        _timer = 0; // Reset timer when deactivated
    }
}

void EffectStrobe::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) {
            output->setValue(0);
        }
        return;
    }
    _timer = (_timer + delta_ms) % _strobe_period_ms;
    uint8_t value = (_timer < _on_time_ms) ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// --- EffectMarsLight ---

EffectMarsLight::EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent)
    : _peak_brightness(peak_brightness), _angle(0.0f) {
    if (oscillation_frequency_mhz == 0) oscillation_frequency_mhz = 1;
    _oscillation_period_ms = 1000.0f / (oscillation_frequency_mhz / 1000.0f); // convert mHz to Hz
    _phase_shift_rad = 2.0f * PI * (phase_shift_percent / 100.0f);
    _angle = _phase_shift_rad;
}

void EffectMarsLight::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) {
            output->setValue(0);
        }
        return;
    }
    float increment = (2.0f * PI / _oscillation_period_ms) * delta_ms;
    _angle += increment;
    if (_angle > (2.0f * PI + _phase_shift_rad)) {
        _angle -= 2.0f * PI;
    }

    float sin_val = (sin(_angle) + 1.0f) / 2.0f;
    uint8_t value = (uint8_t)(sin_val * _peak_brightness);
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// --- EffectSoftStartStop ---

EffectSoftStartStop::EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness)
    : _target_brightness(target_brightness), _current_brightness(0.0f) {
    _fade_in_increment = (fade_in_time_ms > 0) ? (float)_target_brightness / fade_in_time_ms : _target_brightness;
    _fade_out_increment = (fade_out_time_ms > 0) ? (float)_target_brightness / fade_out_time_ms : _target_brightness;
}

void EffectSoftStartStop::setActive(bool active) {
    Effect::setActive(active);
}

void EffectSoftStartStop::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
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
    uint8_t value = (uint8_t)_current_brightness;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}
