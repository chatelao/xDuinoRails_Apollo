#include "FunctionMapping.h"
#include "FunctionManager.h"

bool Condition::evaluate(const FunctionManager& fm) const {
    switch (source) {
        case TriggerSource::FUNC_KEY:
            switch (comparator) {
                case TriggerComparator::IS_ON:
                    return fm.getFunctionState(parameter);
                case TriggerComparator::IS_OFF:
                    return !fm.getFunctionState(parameter);
                default:
                    return false;
            }
        case TriggerSource::DIRECTION:
            switch (comparator) {
                case TriggerComparator::IS_FWD:
                    return fm.getDirection() == DECODER_DIRECTION_FORWARD;
                case TriggerComparator::IS_REV:
                    return fm.getDirection() == DECODER_DIRECTION_REVERSE;
                default:
                    return false;
            }
        case TriggerSource::SPEED:
            switch (comparator) {
                case TriggerComparator::IS_ZERO:
                    return fm.getSpeed() == 0;
                case TriggerComparator::IS_NOT_ZERO:
                    return fm.getSpeed() != 0;
                default:
                    return false;
            }
    }
    return false;
}

bool ConditionVariable::evaluate(const FunctionManager& fm) const {
    for (const auto& condition : conditions) {
        if (!condition.evaluate(fm)) {
            return false; // All conditions must be true
        }
    }
    return true;
}

bool MappingRule::evaluate(const FunctionManager& fm) const {
    // Check positive conditions
    for (uint8_t cv_id : positive_conditions) {
        if (!fm.getConditionVariableState(cv_id)) {
            return false;
        }
    }
    // Check negative conditions
    for (uint8_t cv_id : negative_conditions) {
        if (fm.getConditionVariableState(cv_id)) {
            return false;
        }
    }
    return true;
}
