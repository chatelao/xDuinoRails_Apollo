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

#endif // LIGHT_EFFECT_H
