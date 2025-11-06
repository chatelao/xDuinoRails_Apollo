#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include <vector>
#include "LogicalFunction.h"

/**
 * @file FunctionManager.h
 * @brief Manages all logical functions and maps them to function key events.
 */

class FunctionManager {
public:
    ~FunctionManager();

    /**
     * @brief Add a logical function to be managed.
     * @param function Pointer to the LogicalFunction. The manager takes ownership.
     */
    void addLogicalFunction(LogicalFunction* function);

    /**
     * @brief Update all managed functions. Should be called in the main loop.
     * @param delta_ms Time elapsed since the last update.
     */
    void update(uint32_t delta_ms);

    /**
     * @brief Process a DCC function key event.
     * @param functionNumber The function number (0-28).
     * @param functionState The state of the function (true for on, false for off).
     */
    void setFunctionState(uint8_t functionNumber, bool functionState);

private:
    std::vector<LogicalFunction*> _logical_functions;
    // For Phase 1, we will use a simple, direct mapping.
    // This will be expanded in Phase 2.
    // For now, we assume F-key index matches logical_function index.
};

#endif // FUNCTION_MANAGER_H
