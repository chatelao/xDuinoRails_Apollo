#include "LightEffect.h"

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
