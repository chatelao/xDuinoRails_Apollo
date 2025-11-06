#include "Arduino.h"
#include <stdexcept>

// Global mock object
MockArduino mock;

// Mock function definitions
void pinMode(uint8_t pin, uint8_t mode) {
    mock.pinMode_modes[pin] = mode;
}

void analogWrite(uint8_t pin, int value) {
    mock.analogWrite_values[pin] = value;
}

uint32_t millis() {
    return mock.millis_value;
}

void delay(uint32_t ms) {
    // In a test environment, we don't actually want to wait.
    // We can advance the mock time if needed.
    mock.millis_value += ms;
}
