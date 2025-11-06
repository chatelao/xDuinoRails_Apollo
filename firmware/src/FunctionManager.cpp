#include "FunctionManager.h"

/**
 * @file FunctionManager.cpp
 * @brief Implementation for the FunctionManager class.
 */

FunctionManager::~FunctionManager() {
    for (auto& func : _logical_functions) {
        delete func;
    }
}

void FunctionManager::addLogicalFunction(LogicalFunction* function) {
    _logical_functions.push_back(function);
}

void FunctionManager::setFunctionState(uint8_t functionNumber, bool functionState) {
    // Phase 1: Direct mapping. F-key number maps directly to the index
    // in the _logical_functions vector.
    if (functionNumber < _logical_functions.size()) {
        _logical_functions[functionNumber]->setActive(functionState);
    }
}

void FunctionManager::update(uint32_t delta_ms) {
    for (auto& func : _logical_functions) {
        func->update(delta_ms);
    }
}
