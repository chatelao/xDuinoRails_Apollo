#include "FunctionMapping.h"
#include "FunctionManager.h"

/**
 * @file FunctionMapping.cpp
 * @brief Implementation of the evaluation logic for the mapping system.
 */

bool ConditionVariable::evaluate(const FunctionManager& manager) const {
    // For now, we only support a single condition per variable.
    // This could be extended to support AND/OR logic between multiple conditions.
    if (conditions.empty()) {
        return false;
    }
    const Condition& c = conditions.front();

    uint16_t source_value = 0;
    switch (c.source) {
        case TriggerSource::FUNC_KEY:
            source_value = manager.getFunctionState(c.parameter);
            break;
        case TriggerSource::DIRECTION:
            source_value = (uint16_t)manager.getDirection();
            break;
        case TriggerSource::SPEED:
            source_value = manager.getSpeed();
            break;
        case TriggerSource::LOGICAL_FUNC_STATE:
            // This would require access to the logical functions, which is a potential future enhancement.
            // For now, this is not implemented.
            return false;
        case TriggerSource::BINARY_STATE:
            source_value = manager.getBinaryState(c.parameter);
            break;
    }

    switch (c.comparator) {
        case TriggerComparator::EQ: return source_value == c.parameter;
        case TriggerComparator::NEQ: return source_value != c.parameter;
        case TriggerComparator::GT: return source_value > c.parameter;
        case TriggerComparator::LT: return source_value < c.parameter;
        case TriggerComparator::GTE: return source_value >= c.parameter;
        case TriggerComparator::LTE: return source_value <= c.parameter;
        case TriggerComparator::BIT_AND: return (source_value & c.parameter) != 0;
        case TriggerComparator::IS_TRUE: return source_value != 0;
        default: return false;
    }
}

bool MappingRule::evaluate(const FunctionManager& manager) const {
    // Check positive conditions (all must be true)
    for (uint8_t cv_id : positive_conditions) {
        if (cv_id > 0 && !manager.getConditionVariableState(cv_id)) {
            return false;
        }
    }

    // Check negative conditions (all must be false)
    for (uint8_t cv_id : negative_conditions) {
        if (cv_id > 0 && manager.getConditionVariableState(cv_id)) {
            return false;
        }
    }

    // If we passed all checks, the rule is active.
    return true;
}
