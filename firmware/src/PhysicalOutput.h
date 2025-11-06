#ifndef PHYSICAL_OUTPUT_H
#define PHYSICAL_OUTPUT_H

#include <Arduino.h>

/**
 * @file PhysicalOutput.h
 * @brief Manages a single physical output pin of the microcontroller.
 */

class PhysicalOutput {
public:
    /**
     * @brief Construct a new Physical Output object.
     * @param pin The microcontroller pin number.
     */
    PhysicalOutput(uint8_t pin);

    /**
     * @brief Set the PWM value of the output.
     * @param value The PWM value (0-255).
     */
    void setValue(uint8_t value);

private:
    uint8_t _pin;
};

#endif // PHYSICAL_OUTPUT_H
