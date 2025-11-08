#ifndef FUNCTION_MAPPING_H
#define FUNCTION_MAPPING_H

#include <vector>
#include <cstdint>

/**
 * @file FunctionMapping.h
 * @brief Data structures for the advanced function mapping system.
 */

// --- Enums ---

enum class TriggerSource : uint8_t {
    NONE = 0,
    FUNC_KEY = 1,
    DIRECTION = 2,
    SPEED = 3,
    LOGICAL_FUNC_STATE = 4,
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

// --- Data Structures ---

struct Condition {
    TriggerSource source;
    TriggerComparator comparator;
    uint8_t parameter;
};

// Forward declaration to resolve circular dependency
class FunctionManager;

struct ConditionVariable {
    uint8_t id;
    std::vector<Condition> conditions;
    bool evaluate(const FunctionManager& manager) const;
};

struct MappingRule {
    uint8_t target_logical_function_id;
    std::vector<uint8_t> positive_conditions;
    std::vector<uint8_t> negative_conditions;
    MappingAction action;
    bool evaluate(const FunctionManager& manager) const;
};

#endif // FUNCTION_MAPPING_H
