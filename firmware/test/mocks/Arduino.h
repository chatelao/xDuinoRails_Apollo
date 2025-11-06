#ifndef ARDUINO_MOCK_H
#define ARDUINO_MOCK_H

#include <cstdint>
#include <map>

#define OUTPUT 0x1

// Mock data structures to track function calls
struct MockArduino {
    std::map<uint8_t, uint8_t> pinMode_modes;
    std::map<uint8_t, int> analogWrite_values;
    uint32_t millis_value;

    void reset() {
        pinMode_modes.clear();
        analogWrite_values.clear();
        millis_value = 0;
    }
};

extern MockArduino mock;

// Mock function declarations
void pinMode(uint8_t pin, uint8_t mode);
void analogWrite(uint8_t pin, int value);
uint32_t millis();
void delay(uint32_t ms);

#endif // ARDUINO_MOCK_H
