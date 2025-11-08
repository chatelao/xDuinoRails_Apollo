#include "LogicalFunction.h"

/**
 * @file LogicalFunction.cpp
 * @brief Implementation for the LogicalFunction class.
 */

LogicalFunction::LogicalFunction(Effect* effect) : _effect(effect) {
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

bool LogicalFunction::isActive() const {
    if (_effect) {
        return _effect->isActive();
    }
    return false;
}

void LogicalFunction::setDimmed(bool dimmed) {
    if (_effect) {
        _effect->setDimmed(dimmed);
    }
}

bool LogicalFunction::isDimmed() const {
    if (_effect) {
        return _effect->isDimmed();
    }
    return false;
}

void LogicalFunction::update(uint32_t delta_ms) {
    if (!_effect) {
        return;
    }

    _effect->update(delta_ms, _outputs);
}
