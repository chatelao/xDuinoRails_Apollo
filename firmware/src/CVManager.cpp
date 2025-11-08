#include "CVManager.h"
#include "cv_definitions.h"
#include "Arduino.h" // For EEPROM, etc.

CVManager::CVManager() {}

void CVManager::begin() {
    // For now, we'll just load the defaults. In a production implementation,
    // this would be replaced with a call to loadCVsFromEeprom().
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

    // --- Standard CVs (aligned with RCN-225) ---
    _cv_values[CV_MULTIFUNCTION_PRIMARY_ADDRESS] = DECODER_DEFAULT_PRIMARY_ADDRESS;
    _cv_values[CV_START_VOLTAGE] = DECODER_DEFAULT_START_VOLTAGE;
    _cv_values[CV_ACCELERATION_RATE] = DECODER_DEFAULT_ACCELERATION_RATE;
    _cv_values[CV_DECELERATION_RATE] = DECODER_DEFAULT_DECELERATION_RATE;
    _cv_values[CV_MAXIMUM_SPEED] = DECODER_DEFAULT_MAXIMUM_SPEED;
    _cv_values[CV_MANUFACTURER_ID] = DECODER_DEFAULT_MANUFACTURER_ID;
    _cv_values[CV_DECODER_VERSION_ID] = DECODER_DEFAULT_VERSION_ID;
    _cv_values[CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB] = DECODER_DEFAULT_EXT_ADDRESS_MSB;
    _cv_values[CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB] = DECODER_DEFAULT_EXT_ADDRESS_LSB;
    _cv_values[CV_DECODER_CONFIGURATION] = DECODER_DEFAULT_CV29_CONFIG;

    // --- RCN-225 Function Mapping (CVs 33-46) ---
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 0] = DECODER_DEFAULT_F0_FWD_MAPPING; // CV 33
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 1] = DECODER_DEFAULT_F0_REV_MAPPING; // CV 34
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 2] = DECODER_DEFAULT_F1_MAPPING;   // CV 35
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 3] = DECODER_DEFAULT_F2_MAPPING;   // CV 36
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 4] = DECODER_DEFAULT_F3_MAPPING;   // CV 37
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 5] = DECODER_DEFAULT_F4_MAPPING;   // CV 38
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 6] = DECODER_DEFAULT_F5_MAPPING;   // CV 39
    _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + 7] = DECODER_DEFAULT_F6_MAPPING;   // CV 40
    // CVs 41-46 (F7-F12) default to 0, which means no mapping.
    for (int i = 8; i <= (CV_OUTPUT_LOCATION_CONFIG_END - CV_OUTPUT_LOCATION_CONFIG_START); ++i) {
        _cv_values[CV_OUTPUT_LOCATION_CONFIG_START + i] = 0;
    }

    _cv_values[CV_FUNCTION_MAPPING_METHOD] = DECODER_DEFAULT_FUNCTION_MAPPING_METHOD;
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
