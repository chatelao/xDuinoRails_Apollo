#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include <vector>
#include <map>
#include "LogicalFunction.h"
#include "FunctionMapping.h"

#define MAX_DCC_FUNCTIONS 29

enum DecoderDirection {
    DECODER_DIRECTION_REVERSE = 0,
    DECODER_DIRECTION_FORWARD = 1
};

/**
 * @file FunctionManager.h
 * @brief Manages all logical functions and maps them to function key events.
 */

class FunctionManager {
public:
    FunctionManager();
    ~FunctionManager();

    void addLogicalFunction(LogicalFunction* function);
    void addConditionVariable(const ConditionVariable& cv);
    void addMappingRule(const MappingRule& rule);

    void update(uint32_t delta_ms);

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

    // --- Test Hooks ---
    void reset(); // Clear all configuration
    size_t getLogicalFunctionCount() const { return _logical_functions.size(); }
    size_t getConditionVariableCount() const { return _condition_variables.size(); }
    size_t getMappingRuleCount() const { return _mapping_rules.size(); }
    LogicalFunction* getLogicalFunction(size_t index) { return _logical_functions[index]; }


private:
    void evaluateMapping();

    std::vector<LogicalFunction*> _logical_functions;
    std::vector<ConditionVariable> _condition_variables;
    std::vector<MappingRule> _mapping_rules;

    // --- Decoder State ---
    bool _function_states[MAX_DCC_FUNCTIONS] = {false};
    DecoderDirection _direction = DECODER_DIRECTION_FORWARD;
    uint16_t _speed = 0;
    std::map<uint16_t, bool> m_binary_states;
    std::map<uint8_t, bool> _cv_states; // Cache for evaluated ConditionVariable states
    bool _state_changed = true; // Flag to trigger re-evaluation
};

#endif // FUNCTION_MANAGER_H
