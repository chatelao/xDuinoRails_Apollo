#ifndef EFFECT_H
#define EFFECT_H

#include <Arduino.h>
#include <vector>
#include "PhysicalOutput.h"

/**
 * @file Effect.h
 * @brief Base class for all logical effects (lighting, servo, etc.).
 */

// --- Base Class ---

class Effect {
public:
    virtual ~Effect() {}
    virtual void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) = 0;
    virtual void setActive(bool active) { _is_active = active; }
    virtual bool isActive() const { return _is_active; }
    virtual void setDimmed(bool dimmed) {}
    virtual bool isDimmed() const { return false; }

protected:
    bool _is_active = false;
};

// --- Phase 1 Implementations ---

/**
 * @class EffectSteady
 * @brief A simple, steady light effect with a fixed brightness.
 */
class EffectSteady : public Effect {
public:
    EffectSteady(uint8_t brightness) : _brightness(brightness) {}
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;

private:
    uint8_t _brightness;
};

/**
 * @class EffectDimming
 * @brief A light effect that can be dimmed between two brightness levels.
 */
class EffectDimming : public Effect {
public:
    EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setDimmed(bool dimmed) override;
    bool isDimmed() const override { return _is_dimmed; }

private:
    uint8_t _brightness_full;
    uint8_t _brightness_dimmed;
    bool _is_dimmed = false;
};

// --- Phase 2 Implementations ---

/**
 * @class EffectFlicker
 * @brief Simulates the flickering of a firebox or lantern using Perlin noise.
 */
class EffectFlicker : public Effect {
public:
    EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;

private:
    uint8_t _base_brightness;
    uint8_t _flicker_depth;
    uint8_t _flicker_speed;
    float _noise_position;
    float _noise_increment;
};

/**
 * @class EffectStrobe
 * @brief Simulates a strobe or beacon light.
 */
class EffectStrobe : public Effect {
public:
    EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;

private:
    uint32_t _strobe_period_ms;
    uint32_t _on_time_ms;
    uint8_t _brightness;
    uint32_t _timer;
};

/**
 * @class EffectMarsLight
 * @brief Simulates an oscillating Mars Light or Gyralite using a sine wave.
 */
class EffectMarsLight : public Effect {
public:
    EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;

private:
    float _oscillation_period_ms;
    float _peak_brightness;
    float _phase_shift_rad;
    float _angle;
};

/**
 * @class EffectSoftStartStop
 * @brief Fades the light in and out smoothly.
 */
class EffectSoftStartStop : public Effect {
public:
    EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;

private:
    float _fade_in_increment;  // per ms
    float _fade_out_increment; // per ms
    uint8_t _target_brightness;
    float _current_brightness;
};

// --- Phase 3 Implementations ---

/**
 * @class EffectServo
 * @brief Controls a servo motor, moving it between two endpoints.
 */
class EffectServo : public Effect {
public:
    EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;

private:
    uint8_t _endpoint_a;
    uint8_t _endpoint_b;
    float _speed; // degrees per ms
    float _current_angle;
    float _target_angle;
    bool _is_at_a = true;
};

/**
 * @class EffectSmokeGenerator
 * @brief Controls a smoke generator with a heater and a fan.
 */
class EffectSmokeGenerator : public Effect {
public:
    EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;

private:
    bool _heater_enabled;
    uint8_t _fan_speed;
};

#endif // EFFECT_H
