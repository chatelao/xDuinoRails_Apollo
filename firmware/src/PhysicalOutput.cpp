#include "PhysicalOutput.h"

/**
 * @file PhysicalOutput.cpp
 * @brief Implementation for the PhysicalOutput class.
 */

PhysicalOutput::PhysicalOutput(uint8_t pin) : _pin(pin) {
    pinMode(_pin, OUTPUT);
    analogWrite(_pin, 0);
}

void PhysicalOutput::setValue(uint8_t value) {
    analogWrite(_pin, value);
}
