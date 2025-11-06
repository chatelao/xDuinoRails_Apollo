#ifndef LIGHT_EFFECT_H
#define LIGHT_EFFECT_H

#include <Arduino.h>

/**
 * @file LightEffect.h
 * @brief Base class for lighting effects and simple effect implementations.
 */

// --- Base Class ---

class LightEffect {
public:
    virtual ~LightEffect() {}
    virtual void update(uint32_t delta_ms) = 0;
    virtual uint8_t getPwmValue() = 0;
    virtual void setActive(bool active) { _is_active = active; }
    virtual bool isActive() { return _is_active; }

protected:
    bool _is_active = false;
};

// --- Phase 1 Implementations ---

/**
 * @class EffectSteady
 * @brief A simple, steady light effect with a fixed brightness.
 */
class EffectSteady : public LightEffect {
public:
    EffectSteady(uint8_t brightness) : _brightness(brightness) {}
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;

private:
    uint8_t _brightness;
};

/**
 * @class EffectDimming
 * @brief A light effect that can be dimmed between two brightness levels.
 * @note For Phase 1, this will just toggle between full and dimmed.
 */
class EffectDimming : public LightEffect {
public:
    EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
    void setDimmed(bool dimmed);

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
class EffectFlicker : public LightEffect {
public:
    EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;

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
class EffectStrobe : public LightEffect {
public:
    EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
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
class EffectMarsLight : public LightEffect {
public:
    EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;

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
class EffectSoftStartStop : public LightEffect {
public:
    EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
    void setActive(bool active) override;

private:
    float _fade_in_increment;  // per ms
    float _fade_out_increment; // per ms
    uint8_t _target_brightness;
    float _current_brightness;
};

#endif // LIGHT_EFFECT_H
