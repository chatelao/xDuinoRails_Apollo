#ifndef PHYSICAL_OUTPUT_MANAGER_H
#define PHYSICAL_OUTPUT_MANAGER_H

#include <vector>
#include "PhysicalOutput.h"

/**
 * @file PhysicalOutputManager.h
 * @brief Manages all physical outputs on the decoder.
 */

class PhysicalOutputManager {
public:
    /**
     * @brief Initializes the manager and creates all physical output objects.
     *        This should be called once at startup.
     */
    void begin();

    /**
     * @brief Gets a pointer to a physical output by its ID.
     * @param id The ID of the physical output (0-indexed).
     * @return A pointer to the PhysicalOutput object, or nullptr if not found.
     */
    PhysicalOutput* getOutputById(uint8_t id);

private:
    std::vector<PhysicalOutput> _outputs;
};

#endif // PHYSICAL_OUTPUT_MANAGER_H
