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

void FunctionManager::setBinaryState(uint16_t state_number, bool value) {
    if (m_binary_states.find(state_number) == m_binary_states.end() || m_binary_states[state_number] != value) {
        m_binary_states[state_number] = value;
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

bool FunctionManager::getBinaryState(uint16_t state_number) const {
    auto it = m_binary_states.find(state_number);
    if (it != m_binary_states.end()) {
        return it->second;
    }
    return false; // Default to false if not found
}

void FunctionManager::reset() {
    // Clear all configuration vectors
    for (auto lf : _logical_functions) {
        delete lf; // LogicalFunction owns the Effect pointer
    }
    _logical_functions.clear();
    _condition_variables.clear();
    _mapping_rules.clear();
    _cv_states.clear();
    m_binary_states.clear();

    // Reset state variables
    for (int i = 0; i < MAX_DCC_FUNCTIONS; ++i) {
        _function_states[i] = false;
    }
    _direction = DECODER_DIRECTION_FORWARD;
    _speed = 0;
    _state_changed = true;
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
                    case MappingAction::ACTIVATE:
                        target_func->setActive(true);
                        break;
                    case MappingAction::DEACTIVATE:
                        target_func->setActive(false);
                        break;
                    case MappingAction::SET_DIMMED:
                        target_func->setDimmed(!target_func->isDimmed());
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
