#include "CVManager.h"
#include "Arduino.h" // For EEPROM, etc.

CVManager::CVManager() {}

void CVManager::begin() {
    // For now, we'll just load the defaults.
    // In a real implementation, we would try to load from EEPROM first.
    setDefaultCVs();
}

uint8_t CVManager::readCV(uint16_t cv_number) {
    auto it = _cv_values.find(cv_number);
    if (it != _cv_values.end()) {
        return it->second;
    }
    return 0; // Per NMRA spec, reading an unsupported CV should return 0.
}

void CVManager::writeCV(uint16_t cv_number, uint8_t value) {
    _cv_values[cv_number] = value;
    // TODO: Add call to writeCvToEeprom(cv_number, value);
}

void CVManager::setDefaultCVs() {
    _cv_values.clear();

    // --- Standard CVs (from CV_MAPPING.md) ---
    _cv_values[1] = 3;    // Primary Address
    _cv_values[2] = 80;   // Start Voltage
    _cv_values[3] = 20;   // Acceleration Rate
    _cv_values[4] = 40;   // Deceleration Rate
    _cv_values[8] = 165;  // Manufacturer ID (DIY)

    // --- Default Function Mapping ---
    // This setup creates a direction-dependent headlight on F0.

    // LF 0: Headlight Fwd (Steady, Full Brightness, Output 0)
    _cv_values[CV_BASE_LOGICAL_FUNCTIONS + 0] = 1; // Effect Type: Steady
    _cv_values[CV_BASE_LOGICAL_FUNCTIONS + 1] = 255; // Brightness
    _cv_values[CV_BASE_LOGICAL_FUNCTIONS + 4] = 0; // Physical Output 0

    // LF 1: Headlight Rev (Steady, Full Brightness, Output 1)
    _cv_values[CV_BASE_LOGICAL_FUNCTIONS + 8] = 1; // Effect Type: Steady
    _cv_values[CV_BASE_LOGICAL_FUNCTIONS + 9] = 255; // Brightness
    _cv_values[CV_BASE_LOGICAL_FUNCTIONS + 12] = 1; // Physical Output 1

    // CV 1: F0 is ON
    _cv_values[CV_BASE_COND_VARS + 0] = 1; // Source: Func Key
    _cv_values[CV_BASE_COND_VARS + 1] = 1; // Comparator: IS_ON
    _cv_values[CV_BASE_COND_VARS + 2] = 0; // Parameter: F0

    // CV 2: Direction is FWD
    _cv_values[CV_BASE_COND_VARS + 4] = 2; // Source: Direction
    _cv_values[CV_BASE_COND_VARS + 5] = 3; // Comparator: IS_FWD

    // CV 3: Direction is REV
    _cv_values[CV_BASE_COND_VARS + 8] = 2; // Source: Direction
    _cv_values[CV_BASE_COND_VARS + 9] = 4; // Comparator: IS_REV

    // Rule 0: (F0 ON AND Dir FWD) -> Turn ON LF 0 (Headlight Fwd)
    _cv_values[CV_BASE_MAPPING_RULES + 0] = 1; // Target LF 1 (index 0)
    _cv_values[CV_BASE_MAPPING_RULES + 1] = 1; // Positive CV: CV1 (F0 ON)
    _cv_values[CV_BASE_MAPPING_RULES + 2] = 2; // Positive CV: CV2 (Dir FWD)
    _cv_values[CV_BASE_MAPPING_RULES + 3] = 1; // Action: TURN_ON

    // Rule 1: (F0 ON AND Dir REV) -> Turn ON LF 1 (Headlight Rev)
    _cv_values[CV_BASE_MAPPING_RULES + 4] = 2; // Target LF 2 (index 1)
    _cv_values[CV_BASE_MAPPING_RULES + 5] = 1; // Positive CV: CV1 (F0 ON)
    _cv_values[CV_BASE_MAPPING_RULES + 6] = 3; // Positive CV: CV3 (Dir REV)
    _cv_values[CV_BASE_MAPPING_RULES + 7] = 1; // Action: TURN_ON
}

// --- EEPROM Persistence (Placeholder) ---

void CVManager::loadCVsFromEeprom() {
    // TODO: Implement EEPROM loading logic.
    // This would involve iterating through CVs and reading from EEPROM.
    // A "magic byte" or checksum is recommended to check for valid data.
}

void CVManager::writeCvToEeprom(uint16_t cv_number, uint8_t value) {
    // TODO: Implement EEPROM writing logic.
    // EEPROM has a limited number of write cycles, so this should be
    // handled carefully (e.g., only write if the value has changed).
}
