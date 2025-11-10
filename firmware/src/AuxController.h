#ifndef AUX_CONTROLLER_H
#define AUX_CONTROLLER_H

#include <Arduino.h>
#include <vector>
#include <Servo.h>
#include <cstdint>
#include <map>
#include "CVManager.h"

#define MAX_DCC_FUNCTIONS 29

enum DecoderDirection {
    DECODER_DIRECTION_REVERSE = 0,
    DECODER_DIRECTION_FORWARD = 1
};

// --- Forward Declarations ---
class AuxController;

// --- Physical Outputs ---

enum class OutputType {
    PWM,
    SERVO
};

class PhysicalOutput {
public:
    PhysicalOutput(uint8_t pin, OutputType type);
    void attach();
    void setValue(uint8_t value);
    void setServoAngle(uint16_t angle);

private:
    uint8_t _pin;
    OutputType _type;
    Servo _servo;
};


// --- Effects ---

class Effect {
public:
    virtual ~Effect() {}
    virtual void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) = 0;
    virtual void setActive(bool active) { _is_active = active; }
    virtual bool isActive() const { return _is_active; }
    virtual void setDimmed(bool dimmed) {}
    virtual bool isDimmed() const { return false; }

protected:
    bool _is_active = false;
};

class EffectSteady : public Effect {
public:
    EffectSteady(uint8_t brightness) : _brightness(brightness) {}
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;

private:
    uint8_t _brightness;
};

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

class EffectSmokeGenerator : public Effect {
public:
    EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed);
    void update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) override;

private:
    bool _heater_enabled;
    uint8_t _fan_speed;
};


// --- Logical Functions ---

class LogicalFunction {
public:
    LogicalFunction(Effect* effect);
    ~LogicalFunction();
    void addOutput(PhysicalOutput* output);
    void update(uint32_t delta_ms);
    void setActive(bool active);
    bool isActive() const;
    void setDimmed(bool dimmed);
    bool isDimmed() const;

private:
    Effect* _effect;
    std::vector<PhysicalOutput*> _outputs;
};


// --- Function Mapping ---

enum class TriggerSource : uint8_t {
    NONE = 0,
    FUNC_KEY = 1,
    DIRECTION = 2,
    SPEED = 3,
    LOGICAL_FUNC_STATE = 4,
    BINARY_STATE = 5,
};

enum class TriggerComparator : uint8_t {
    NONE = 0,
    EQ = 1,
    NEQ = 2,
    GT = 3,
    LT = 4,
    GTE = 5,
    LTE = 6,
    BIT_AND = 7,
    IS_TRUE = 8,
};

enum class MappingAction : uint8_t {
    NONE = 0,
    ACTIVATE = 1,
    DEACTIVATE = 2,
    SET_DIMMED = 3,
};

enum class FunctionMappingMethod : uint8_t {
    PROPRIETARY = 0,
    RCN_225 = 1,
    RCN_227_PER_FUNCTION = 2,
    RCN_227_PER_OUTPUT_V1 = 3,
    RCN_227_PER_OUTPUT_V2 = 4,
    RCN_227_PER_OUTPUT_V3 = 5,
};

struct Condition {
    TriggerSource source;
    TriggerComparator comparator;
    uint8_t parameter;
};

struct ConditionVariable {
    uint8_t id;
    std::vector<Condition> conditions;
    bool evaluate(const AuxController& controller) const;
};

struct MappingRule {
    uint8_t target_logical_function_id;
    std::vector<uint8_t> positive_conditions;
    std::vector<uint8_t> negative_conditions;
    MappingAction action;
    bool evaluate(const AuxController& controller) const;
};


// --- Main Controller Class ---

class AuxController {
public:
    AuxController();
    ~AuxController();

    void begin();
    void update(uint32_t delta_ms);
    void loadFromCVs(CVManager& cvManager);

    // --- State Update Methods ---
    void setFunctionState(uint8_t functionNumber, bool functionState);
    void setDirection(DecoderDirection direction);
    void setSpeed(uint16_t speed);
    void setBinaryState(uint16_t state_number, bool value);

    // --- State Getter Methods (for evaluation) ---
    bool getFunctionState(uint8_t functionNumber) const;
    DecoderDirection getDirection() const;
    uint16_t getSpeed() const;
    bool getConditionVariableState(uint8_t cv_id) const;
    bool getBinaryState(uint16_t state_number) const;
    LogicalFunction* getLogicalFunction(size_t index);

private:
    void addLogicalFunction(LogicalFunction* function);
    void addConditionVariable(const ConditionVariable& cv);
    void addMappingRule(const MappingRule& rule);
    void reset();

    void evaluateMapping();
    PhysicalOutput* getOutputById(uint8_t id);

    // --- CV Loading ---
    void parseRcn225(CVManager& cvManager);
    void parseRcn227PerFunction(CVManager& cvManager);
    void parseRcn227PerOutputV1(CVManager& cvManager);
    void parseRcn227PerOutputV2(CVManager& cvManager);
    void parseRcn227PerOutputV3(CVManager& cvManager);

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

#endif // AUX_CONTROLLER_H
