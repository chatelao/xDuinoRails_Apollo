#include "LogicalFunction.h"

/**
 * @file LogicalFunction.cpp
 * @brief Implementation for the LogicalFunction class.
 */

LogicalFunction::LogicalFunction(LightEffect* effect) : _effect(effect) {
    // The LogicalFunction takes ownership of the effect pointer.
}

LogicalFunction::~LogicalFunction() {
    delete _effect; // Prevent memory leaks
}

void LogicalFunction::addOutput(PhysicalOutput* output) {
    _outputs.push_back(output);
}

void LogicalFunction::setActive(bool active) {
    if (_effect) {
        _effect->setActive(active);
    }
}

void LogicalFunction::setDimmed(bool dimmed) {
    if (_effect) {
        // Use dynamic_cast to safely check if the effect is an EffectDimming
        if (EffectDimming* dimmable_effect = dynamic_cast<EffectDimming*>(_effect)) {
            dimmable_effect->setDimmed(dimmed);
        }
    }
}

void LogicalFunction::update(uint32_t delta_ms) {
    if (!_effect) {
        return;
    }

    _effect->update(delta_ms);
    uint8_t pwmValue = _effect->getPwmValue();

    for (auto& output : _outputs) {
        output->setValue(pwmValue);
    }
}
