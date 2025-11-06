#include "PhysicalOutputManager.h"
#include "config.h"

void PhysicalOutputManager::begin() {
    // Create all the physical output objects based on the config.
    // The ID of each output is determined by its position in the vector.
    _outputs.emplace_back(PO_HEADLIGHT_FWD);
    _outputs.emplace_back(PO_HEADLIGHT_REV);
    _outputs.emplace_back(PO_CABIN_LIGHT);
    // Add other physical outputs from config.h here...
}

PhysicalOutput* PhysicalOutputManager::getOutputById(uint8_t id) {
    if (id < _outputs.size()) {
        return &_outputs[id];
    }
    return nullptr;
}
