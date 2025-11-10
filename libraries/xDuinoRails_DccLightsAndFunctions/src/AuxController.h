/**
 * @file AuxController.h
 * @brief Consolidates all logic for auxiliary functions, effects, and physical outputs.
 *
 * This file contains the declaration of the main AuxController class, as well as all
 * related helper classes, enums, and structs that make up the auxiliary function
 * control system. This includes physical output management, the effect system,
 * logical functions, and the CV-based function mapping logic.
 */
#ifndef XDRAILS_DCC_LIGHTS_AND_FUNCTIONS_H
#define XDRAILS_DCC_LIGHTS_AND_FUNCTIONS_H

#include <Arduino.h>
#include <vector>
#include <Servo.h>
#include <cstdint>
#include <map>

#define MAX_DCC_FUNCTIONS 29

namespace xDuinoRails {

/**
 * @enum DecoderDirection
 * @brief Defines the direction of the decoder.
 */
enum DecoderDirection {
    DECODER_DIRECTION_REVERSE = 0, ///< The decoder is moving in reverse.
    DECODER_DIRECTION_FORWARD = 1  ///< The decoder is moving forward.
};

// --- Forward Declarations ---
class AuxController;

/**
 * @class ICVAccess
 * @brief Abstract interface for accessing Configuration Variables (CVs).
 *
 * This class defines the contract for reading and writing CVs, allowing the
 * AuxController to be independent of the specific CV management implementation.
 */
class ICVAccess {
public:
    virtual ~ICVAccess() {}
    /**
     * @brief Reads a CV value.
     * @param cv_number The CV number to read.
     * @return The 8-bit value of the CV.
     */
    virtual uint8_t readCV(uint16_t cv_number) = 0;

    /**
     * @brief Writes a value to a CV.
     * @param cv_number The CV number to write to.
     * @param value The 8-bit value to write.
     */
    virtual void writeCV(uint16_t cv_number, uint8_t value) = 0;
};

// --- Physical Outputs ---

/**
 * @enum OutputType
 * @brief Defines the type of a physical output pin.
 */
enum class OutputType {
    PWM,   ///< A Pulse-Width Modulation (PWM) output, for LEDs etc.
    SERVO  ///< A servo output.
};

/**
 * @class PhysicalOutput
 * @brief Manages a single physical output pin of the microcontroller.
 */
class PhysicalOutput {
public:
    /**
     * @brief Construct a new Physical Output object.
     * @param pin The microcontroller pin number.
     * @param type The type of the output.
     */
    PhysicalOutput(uint8_t pin, OutputType type);

    /**
     * @brief Attaches the output to its pin, setting pinMode or attaching servo.
     */
    void attach();

    /**
     * @brief Sets the PWM value of the output.
     * @param value The PWM value (0-255).
     */
    void setValue(uint8_t value);

    /**
     * @brief Sets the servo angle of the output.
     * @param angle The angle in degrees.
     */
    void setServoAngle(uint16_t angle);

private:
    uint8_t _pin;
    OutputType _type;
    Servo _servo;
};


// --- Effects ---

/**
 * @class Effect
 * @brief Base class for all logical effects (lighting, servo, etc.).
 */
class Effect {
public:
    /** @brief Virtual destructor. */
    virtual ~Effect() {}

    /**
     * @brief Updates the effect's state. Called every loop.
     * @param delta_ms Time elapsed since the last update.
     * @param outputs A vector of physical outputs to control.
     */
    virtual void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) = 0;

    /**
     * @brief Activates or deactivates the effect.
     * @param active True to activate, false to deactivate.
     */
    virtual void setActive(bool active) { _is_active = active; }

    /**
     * @brief Checks if the effect is currently active.
     * @return True if active, false otherwise.
     */
    virtual bool isActive() const { return _is_active; }

    /**
     * @brief Sets the dimmed state of the effect.
     * @param dimmed True to set to dimmed, false for full brightness.
     */
    virtual void setDimmed(bool dimmed) {}

    /**
     * @brief Checks if the effect is currently dimmed.
     * @return True if dimmed, false otherwise.
     */
    virtual bool isDimmed() const { return false; }

protected:
    bool _is_active = false;
};

/** @class EffectSteady @brief A simple, steady light effect with a fixed brightness. */
class EffectSteady : public Effect {
public:
    EffectSteady(uint8_t brightness) : _brightness(brightness) {}
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    uint8_t _brightness;
};

/** @class EffectDimming @brief A light effect that can be dimmed between two brightness levels. */
class EffectDimming : public Effect {
public:
    EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setDimmed(bool dimmed) override;
    bool isDimmed() const override { return _is_dimmed; }
private:
    uint8_t _brightness_full;
    uint8_t _brightness_dimmed;
    bool _is_dimmed = false;
};

/** @class EffectFlicker @brief Simulates the flickering of a firebox or lantern using Perlin noise. */
class EffectFlicker : public Effect {
public:
    EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    uint8_t _base_brightness;
    uint8_t _flicker_depth;
    uint8_t _flicker_speed;
    float _noise_position;
    float _noise_increment;
};

/** @class EffectStrobe @brief Simulates a strobe or beacon light. */
class EffectStrobe : public Effect {
public:
    EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;
private:
    uint32_t _strobe_period_ms;
    uint32_t _on_time_ms;
    uint8_t _brightness;
    uint32_t _timer;
};

/** @class EffectMarsLight @brief Simulates an oscillating Mars Light or Gyralite using a sine wave. */
class EffectMarsLight : public Effect {
public:
    EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    float _oscillation_period_ms;
    float _peak_brightness;
    float _phase_shift_rad;
    float _angle;
};

/** @class EffectSoftStartStop @brief Fades the light in and out smoothly. */
class EffectSoftStartStop : public Effect {
public:
    EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;
private:
    float _fade_in_increment;
    float _fade_out_increment;
    uint8_t _target_brightness;
    float _current_brightness;
};

/** @class EffectServo @brief Controls a servo motor, moving it between two endpoints. */
class EffectServo : public Effect {
public:
    EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
    void setActive(bool active) override;
private:
    uint8_t _endpoint_a;
    uint8_t _endpoint_b;
    float _speed;
    float _current_angle;
    float _target_angle;
    bool _is_at_a = true;
};

/** @class EffectSmokeGenerator @brief Controls a smoke generator with a heater and a fan. */
class EffectSmokeGenerator : public Effect {
public:
    EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;
private:
    bool _heater_enabled;
    uint8_t _fan_speed;
};


// --- Logical Functions ---

/**
 * @class LogicalFunction
 * @brief Represents a logical feature (e.g., "Front Headlight") and maps it to an effect and physical outputs.
 */
class LogicalFunction {
public:
    /**
     * @brief Construct a new Logical Function object.
     * @param effect Pointer to the Effect to use. The LogicalFunction takes ownership.
     */
    LogicalFunction(Effect* effect);
    /** @brief Destructor. Deletes the owned Effect pointer. */
    ~LogicalFunction();

    /**
     * @brief Adds a physical output that this function should control.
     * @param output Pointer to the PhysicalOutput object.
     */
    void addOutput(PhysicalOutput* output);

    /**
     * @brief Updates the function's state and its physical outputs.
     * @param delta_ms Time elapsed since the last update.
     */
    void update(uint32_t delta_ms);

    /**
     * @brief Activates or deactivates the function's effect.
     * @param active True to activate, false to deactivate.
     */
    void setActive(bool active);
    /** @brief Checks if the function's effect is active. @return True if active. */
    bool isActive() const;

    /**
     * @brief Sets the dimmed state of the function's effect.
     * @param dimmed True to set to dimmed, false for full brightness.
     */
    void setDimmed(bool dimmed);
    /** @brief Checks if the function's effect is dimmed. @return True if dimmed. */
    bool isDimmed() const;

private:
    Effect* _effect;
    std::vector<PhysicalOutput*> _outputs;
};


// --- Function Mapping ---

/** @enum TriggerSource @brief Defines the source of a condition trigger for function mapping. */
enum class TriggerSource : uint8_t {
    NONE = 0,               ///< No source.
    FUNC_KEY = 1,           ///< DCC Function Key state.
    DIRECTION = 2,          ///< Decoder direction of travel.
    SPEED = 3,              ///< Decoder speed.
    LOGICAL_FUNC_STATE = 4, ///< The active state of another logical function.
    BINARY_STATE = 5,       ///< A user-defined binary state (per RCN-227).
};

/** @enum TriggerComparator @brief Defines the comparison to use for a trigger condition. */
enum class TriggerComparator : uint8_t {
    NONE = 0,     ///< No comparison.
    EQ = 1,       ///< Equal to.
    NEQ = 2,      ///< Not equal to.
    GT = 3,       ///< Greater than.
    LT = 4,       ///< Less than.
    GTE = 5,      ///< Greater than or equal to.
    LTE = 6,      ///< Less than or equal to.
    BIT_AND = 7,  ///< Bitwise AND is non-zero.
    IS_TRUE = 8,  ///< The source value is true (for booleans).
};

/** @enum MappingAction @brief Defines the action to take when a mapping rule is satisfied. */
enum class MappingAction : uint8_t {
    NONE = 0,       ///< No action.
    ACTIVATE = 1,   ///< Activate the target logical function.
    DEACTIVATE = 2, ///< Deactivate the target logical function.
    SET_DIMMED = 3, ///< Toggle the dimmed state of the target logical function.
};

/** @enum FunctionMappingMethod @brief Defines which CV-based function mapping standard to use. */
enum class FunctionMappingMethod : uint8_t {
    PROPRIETARY = 0,            ///< A proprietary, non-standard mapping method.
    RCN_225 = 1,                ///< RCN-225 Basic Function Mapping.
    RCN_227_PER_FUNCTION = 2,   ///< RCN-227 (unused).
    RCN_227_PER_OUTPUT_V1 = 3,  ///< RCN-227 System per Output V1 (Matrix).
    RCN_227_PER_OUTPUT_V2 = 4,  ///< RCN-227 System per Output V2 (Function Number).
    RCN_227_PER_OUTPUT_V3 = 5,  ///< RCN-227 System per Output V3 (Function or Binary State Number).
};

/** @struct Condition @brief Defines a single condition to be evaluated in the mapping system. */
struct Condition {
    TriggerSource source;         ///< The source of the data to check (e.g., a function key).
    TriggerComparator comparator; ///< The comparison to perform.
    uint8_t parameter;            ///< The value to compare against.
};

/** @struct ConditionVariable @brief A set of Conditions that are evaluated together with AND logic. */
struct ConditionVariable {
    uint8_t id;                     ///< A unique ID for this variable.
    std::vector<Condition> conditions; ///< The list of conditions to evaluate.
    /** @brief Evaluates all conditions. @param controller The AuxController to get state from. @return True if all conditions pass. */
    bool evaluate(const AuxController& controller) const;
};

/** @struct MappingRule @brief A rule that links condition variables to an action on a logical function. */
struct MappingRule {
    uint8_t target_logical_function_id;     ///< The ID of the LogicalFunction to act upon.
    std::vector<uint8_t> positive_conditions; ///< List of ConditionVariable IDs that must be true.
    std::vector<uint8_t> negative_conditions; ///< List of ConditionVariable IDs that must be false.
    MappingAction action;                     ///< The action to perform if the rule passes.
    /** @brief Evaluates the rule. @param controller The AuxController to get state from. @return True if the rule passes. */
    bool evaluate(const AuxController& controller) const;
};


// --- Main Controller Class ---

/**
 * @class AuxController
 * @brief Central controller for all auxiliary functions, lighting, and other effects.
 *
 * This class manages all physical outputs, logical functions, effects, and the
 * complex mapping logic that ties them together based on decoder state and CV settings.
 */
class AuxController {
public:
    /** @brief Construct a new Aux Controller object. */
    AuxController();
    /** @brief Destructor. Cleans up dynamically allocated resources. */
    ~AuxController();

    /**
     * @brief Adds and initializes a physical output. Must be called for each output pin in setup().
     * @param pin The microcontroller pin number.
     * @param type The type of the output (PWM or SERVO).
     */
    void addPhysicalOutput(uint8_t pin, OutputType type);

    /**
     * @brief Updates the state of all logical functions and effects. Call every loop.
     * @param delta_ms Time elapsed since the last update in milliseconds.
     */
    void update(uint32_t delta_ms);
    /**
     * @brief Loads the entire function mapping configuration from CVs.
     * @param cvAccess A reference to an object that implements the ICVAccess interface.
     */
    void loadFromCVs(ICVAccess& cvAccess);

    // --- State Update Methods ---
    /**
     * @brief Sets the state of a DCC function key.
     * @param functionNumber The function number (0-28).
     * @param functionState True if the function is on, false if off.
     */
    void setFunctionState(uint8_t functionNumber, bool functionState);
    /**
     * @brief Sets the decoder's direction of travel.
     * @param direction The new direction.
     */
    void setDirection(DecoderDirection direction);
    /**
     * @brief Sets the decoder's current speed.
     * @param speed The new speed value.
     */
    void setSpeed(uint16_t speed);
    /**
     * @brief Sets the value of a binary state.
     * @param state_number The ID of the binary state.
     * @param value The new boolean value.
     */
    void setBinaryState(uint16_t state_number, bool value);

    // --- State Getter Methods (for evaluation) ---
    /**
     * @brief Gets the current state of a DCC function key.
     * @param functionNumber The function number to check.
     * @return True if the function is on, false otherwise.
     */
    bool getFunctionState(uint8_t functionNumber) const;
    /** @brief Gets the decoder's current direction. @return The current direction. */
    DecoderDirection getDirection() const;
    /** @brief Gets the decoder's current speed. @return The current speed. */
    uint16_t getSpeed() const;
    /**
     * @brief Gets the evaluated state of a ConditionVariable.
     * @param cv_id The ID of the ConditionVariable.
     * @return The cached boolean result of the variable's evaluation.
     */
    bool getConditionVariableState(uint8_t cv_id) const;
    /**
     * @brief Gets the current value of a binary state.
     * @param state_number The ID of the binary state to check.
     * @return The current boolean value of the state.
     */
    bool getBinaryState(uint16_t state_number) const;
    /**
     * @brief Gets a pointer to a logical function by its index.
     * @param index The index of the logical function.
     * @return A pointer to the LogicalFunction, or nullptr if not found.
     */
    LogicalFunction* getLogicalFunction(size_t index);

#ifdef UNIT_TEST
public:
#endif
private:
    void addLogicalFunction(LogicalFunction* function);
    void addConditionVariable(const ConditionVariable& cv);
    void addMappingRule(const MappingRule& rule);
    void reset();

    void evaluateMapping();
    PhysicalOutput* getOutputById(uint8_t id);

    // --- CV Loading ---
    void parseRcn225(ICVAccess& cvAccess);
    void parseRcn227PerFunction(ICVAccess& cvAccess);
    void parseRcn227PerOutputV1(ICVAccess& cvAccess);
    void parseRcn227PerOutputV2(ICVAccess& cvAccess);
    void parseRcn227PerOutputV3(ICVAccess& cvAccess);

    std::vector<PhysicalOutput> _outputs;
    std::vector<LogicalFunction*> _logical_functions;
    std::vector<ConditionVariable> _condition_variables;
    std::vector<MappingRule> _mapping_rules;

    // --- Decoder State ---
    bool _function_states[MAX_DCC_FUNCTIONS] = {false};
    DecoderDirection _direction = DECODER_DIRECTION_FORWARD;
    uint16_t _speed = 0;
    std::map<uint16_t, bool> m_binary_states;
    std::map<uint8_t, bool> _cv_states;
    bool _state_changed = true;
};

} // namespace xDuinoRails

#endif // XDRAILS_DCC_LIGHTS_AND_FUNCTIONS_H
