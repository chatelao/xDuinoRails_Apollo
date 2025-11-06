#include "FunctionManager.h"
#include "LogicalFunction.h"

FunctionManager::FunctionManager() {}

FunctionManager::~FunctionManager() {
    for (auto& func : _logical_functions) {
        delete func;
    }
    _logical_functions.clear();
}

void FunctionManager::addLogicalFunction(LogicalFunction* function) {
    _logical_functions.push_back(function);
}

void FunctionManager::addConditionVariable(const ConditionVariable& cv) {
    _condition_variables.push_back(cv);
}

void FunctionManager::addMappingRule(const MappingRule& rule) {
    _mapping_rules.push_back(rule);
}

void FunctionManager::update(uint32_t delta_ms) {
    if (_state_changed) {
        evaluateMapping();
        _state_changed = false;
    }

    for (auto& func : _logical_functions) {
        func->update(delta_ms);
    }
}

void FunctionManager::setFunctionState(uint8_t functionNumber, bool functionState) {
    if (functionNumber < MAX_DCC_FUNCTIONS && _function_states[functionNumber] != functionState) {
        _function_states[functionNumber] = functionState;
        _state_changed = true;
    }
}

void FunctionManager::setDirection(DecoderDirection direction) {
    if (_direction != direction) {
        _direction = direction;
        _state_changed = true;
    }
}

void FunctionManager::setSpeed(uint16_t speed) {
    if (_speed != speed) {
        _speed = speed;
        _state_changed = true;
    }
}

bool FunctionManager::getFunctionState(uint8_t functionNumber) const {
    if (functionNumber < MAX_DCC_FUNCTIONS) {
        return _function_states[functionNumber];
    }
    return false;
}

DecoderDirection FunctionManager::getDirection() const {
    return _direction;
}

uint16_t FunctionManager::getSpeed() const {
    return _speed;
}

bool FunctionManager::getConditionVariableState(uint8_t cv_id) const {
    auto it = _cv_states.find(cv_id);
    if (it != _cv_states.end()) {
        return it->second;
    }
    return false; // Default to false if not found
}

void FunctionManager::evaluateMapping() {
    // 1. Evaluate all ConditionVariables and cache their states
    _cv_states.clear();
    for (const auto& cv : _condition_variables) {
        _cv_states[cv.id] = cv.evaluate(*this);
    }

    // 2. Evaluate all MappingRules and apply actions
    for (const auto& rule : _mapping_rules) {
        if (rule.evaluate(*this)) {
            if (rule.target_logical_function_id < _logical_functions.size()) {
                LogicalFunction* target_func = _logical_functions[rule.target_logical_function_id];
                switch (rule.action) {
                    case MappingAction::TURN_ON:
                        target_func->setActive(true);
                        break;
                    case MappingAction::TURN_OFF:
                        target_func->setActive(false);
                        break;
                    case MappingAction::TOGGLE:
                        target_func->setActive(!target_func->isActive());
                        break;
                    case MappingAction::SET_DIMMED_ON:
                        // This requires the target function to have a dimmable effect.
                        // A more robust implementation would use dynamic_cast or a capability flag.
                        target_func->setDimmed(true);
                        break;
                    case MappingAction::SET_DIMMED_OFF:
                        target_func->setDimmed(false);
                        break;
                }
            }
        }
    }
}
