#ifndef PHYSICAL_OUTPUT_H
#define PHYSICAL_OUTPUT_H

#include <Arduino.h>
#include <Servo.h>

/**
 * @file PhysicalOutput.h
 * @brief Manages a single physical output pin of the microcontroller.
 */

enum class OutputType {
    PWM,
    SERVO
};

class PhysicalOutput {
public:
    /**
     * @brief Construct a new Physical Output object.
     * @param pin The microcontroller pin number.
     * @param type The type of the output.
     */
    PhysicalOutput(uint8_t pin, OutputType type);

    /**
     * @brief Attach the output to its pin.
     */
    void attach();

    /**
     * @brief Set the PWM value of the output.
     * @param value The PWM value (0-255).
     */
    void setValue(uint8_t value);

    /**
     * @brief Set the servo angle of the output.
     * @param angle The angle in degrees.
     */
    void setServoAngle(uint16_t angle);

private:
    uint8_t _pin;
    OutputType _type;
    Servo _servo;
};

#endif // PHYSICAL_OUTPUT_H
