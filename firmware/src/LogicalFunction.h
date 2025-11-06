#ifndef LOGICAL_FUNCTION_H
#define LOGICAL_FUNCTION_H

#include <vector>
#include "PhysicalOutput.h"
#include "LightEffect.h"

/**
 * @file LogicalFunction.h
 * @brief Represents a logical feature (e.g., "Front Headlight")
 *        and maps it to physical outputs.
 */

class LogicalFunction {
public:
    /**
     * @brief Construct a new Logical Function object.
     * @param effect Pointer to the LightEffect to use. The LogicalFunction takes ownership.
     */
    LogicalFunction(LightEffect* effect);
    ~LogicalFunction();

    /**
     * @brief Add a physical output that this function should control.
     * @param output Pointer to the PhysicalOutput object.
     */
    void addOutput(PhysicalOutput* output);

    /**
     * @brief Update the function's state and its physical outputs.
     * @param delta_ms Time elapsed since the last update.
     */
    void update(uint32_t delta_ms);

    /**
     * @brief Activate or deactivate the function.
     * @param active True to activate, false to deactivate.
     */
    void setActive(bool active);

    /**
     * @brief Set the dimmed state of the function's effect.
     * @param dimmed True to set to dimmed, false for full brightness.
     */
    void setDimmed(bool dimmed);

private:
    LightEffect* _effect;
    std::vector<PhysicalOutput*> _outputs;
};

#endif // LOGICAL_FUNCTION_H
