#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include <Arduino.h>
#include <vector>

// --- Core Effect Classes ---

/**
 * @file FunctionManager.h
 * @brief Defines the classes for managing lighting and auxiliary functions.
 */

/**
 * @class LightEffect
 * @brief Abstract base class for all light and function effects.
 */
class LightEffect {
public:
    virtual ~LightEffect() {}
    virtual void update(uint32_t delta_ms) = 0;
    virtual uint8_t getPwmValue() = 0;
    virtual void setActive(bool active) { this->active = active; }
    bool isActive() const { return active; }

protected:
    bool active = false;
};

/**
 * @class EffectSteady
 * @brief Implements a "steady on" effect with constant brightness.
 */
class EffectSteady : public LightEffect {
public:
    EffectSteady(uint8_t brightness);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
private:
    uint8_t brightness;
};

/**
 * @class EffectDimming
 * @brief Implements an effect for switching between two brightness levels.
 */
class EffectDimming : public LightEffect {
public:
    EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
    void setDimmed(bool dimmed);
private:
    uint8_t brightness_full;
    uint8_t brightness_dimmed;
    bool is_dimmed = false;
};


// --- Core Infrastructure Classes ---

/**
 * @class PhysicalOutput
 * @brief Represents a physical output pin of the microcontroller.
 */
class PhysicalOutput {
public:
    PhysicalOutput(uint8_t pin);
    void begin();
    void write(uint8_t value);
private:
    uint8_t pin;
    uint8_t last_value = 255; // Initial value to force a first write
};

/**
 * @class LogicalFunction
 * @brief Connects an effect to a physical output.
 */
class LogicalFunction {
public:
    LogicalFunction(PhysicalOutput* output, LightEffect* effect);
    ~LogicalFunction();
    void update();
    void setActive(bool active);
private:
    PhysicalOutput* output;
    LightEffect* effect;
    uint32_t last_update_ms = 0;
};


// --- Management Class ---

#define MAX_LOGICAL_FUNCTIONS 16 // Supports F0-F15

/**
 * @class FunctionManager
 * @brief Manages all logical functions and their lifecycles.
 */
class FunctionManager {
public:
    FunctionManager();
    ~FunctionManager();

    /**
     * @brief Registers a new logical function and maps it to a function key.
     * @param function_key The function key (0-28) that triggers this function.
     * @param output Pointer to the PhysicalOutput object.
     * @param effect Pointer to the LightEffect object.
     * @param direction_dependency Specifies if this function is direction-dependent.
     *        0 = always active (if key is on), 1 = only when forward, -1 = only when reverse.
     */
    void registerFunction(uint8_t function_key, PhysicalOutput* output, LightEffect* effect, int8_t direction_dependency = 0);

    /**
     * @brief Initializes all associated outputs.
     */
    void begin();

    /**
     * @brief Updates the state of all registered logical functions.
     *        Should be called in the main loop.
     */
    void update();

    /**
     * @brief Sets the state of a function key (pressed/not pressed).
     * @param function_key The number of the function key (0-28).
     * @param state The state (true for on, false for off).
     */
    void setFunctionKeyState(uint8_t function_key, bool state);

    /**
     * @brief Sets the current direction of the decoder.
     * @param forward True for forward, false for reverse.
     */
    void setDirection(bool forward);

private:
    struct FunctionMapping {
        LogicalFunction* logical_function;
        int8_t direction_dependency;
        bool key_state;
    };

    std::vector<FunctionMapping*> function_map[MAX_LOGICAL_FUNCTIONS];
    bool current_direction_is_forward = true;
};

#endif // FUNCTION_MANAGER_H
