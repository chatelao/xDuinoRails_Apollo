#ifndef FUNCTION_MAPPING_H
#define FUNCTION_MAPPING_H

#include <Arduino.h>
#include <vector>

// Forward declaration to avoid circular dependency
class FunctionManager;

/**
 * @enum TriggerSource
 * @brief Defines the source of a condition (e.g., a function key, direction).
 */
enum class TriggerSource {
    FUNC_KEY,
    DIRECTION,
    SPEED
};

/**
 * @enum TriggerComparator
 * @brief Defines how to evaluate the trigger source.
 */
enum class TriggerComparator {
    IS_ON,
    IS_OFF,
    IS_FWD,
    IS_REV,
    IS_ZERO,
    IS_NOT_ZERO
};

/**
 * @struct Condition
 * @brief Represents a single condition, e.g., "F1 is ON" or "Speed is > 0".
 *        This is a building block for a ConditionVariable.
 */
struct Condition {
    TriggerSource source;
    uint8_t parameter; // e.g., function key number for FUNC_KEY
    TriggerComparator comparator;

    /**
     * @brief Evaluates this single condition against the decoder's current state.
     * @param fm A const reference to the FunctionManager which holds the state.
     * @return True if the condition is met, false otherwise.
     */
    bool evaluate(const FunctionManager& fm) const;
};

/**
 * @struct ConditionVariable
 * @brief A user-defined boolean variable (e.g., C1) that is true only if all
 *        of its constituent Conditions are true (logical AND).
 */
struct ConditionVariable {
    uint8_t id; // The numeric ID, e.g., 1 for C1.
    std::vector<Condition> conditions;

    /**
     * @brief Evaluates the Condition Variable by evaluating all its Conditions.
     * @param fm A const reference to the FunctionManager.
     * @return True if all conditions are met, false otherwise.
     */
    bool evaluate(const FunctionManager& fm) const;
};

/**
 * @enum MappingAction
 * @brief Defines the action to be taken on a target LogicalFunction when a rule matches.
 */
enum class MappingAction {
    TURN_ON,
    TURN_OFF,
    SET_DIMMED_ON,
    SET_DIMMED_OFF,
    TOGGLE
};

/**
 * @struct MappingRule
 * @brief A single row in the final logic table. It defines which logical function
 *        to act upon if a boolean combination of ConditionVariables is true.
 *        Example: "IF (C1 AND C2 AND NOT C3) THEN Activate 'Yard Mode Light'"
 */
struct MappingRule {
    std::vector<uint8_t> positive_conditions; // List of ConditionVariable IDs that must be true.
    std::vector<uint8_t> negative_conditions; // List of ConditionVariable IDs that must be false (NOT).
    uint8_t target_logical_function_id;
    MappingAction action;

    /**
     * @brief Evaluates the rule against the current state of all ConditionVariables.
     * @param fm A const reference to the FunctionManager.
     * @return True if the rule's conditions are met, false otherwise.
     */
    bool evaluate(const FunctionManager& fm) const;
};

#endif // FUNCTION_MAPPING_H
