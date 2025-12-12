/**
 * @file CVManager.cpp
 * @brief Implements the CVManager class for handling decoder Configuration Variables.
 */
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
    uint16_t mapped_address = getMappedCvAddress(cv_number);
    auto it = _cv_values.find(mapped_address);
    if (it != _cv_values.end()) {
        return it->second;
    }
    return 0; // Per NMRA spec, reading an unsupported CV should return 0.
}

void CVManager::writeCV(uint16_t cv_number, uint8_t value) {
    uint16_t mapped_address = getMappedCvAddress(cv_number);
    _cv_values[mapped_address] = value;
    // TODO: Add call to writeCvToEeprom(mapped_address, value);
}

uint16_t CVManager::getMappedCvAddress(uint16_t cv_number) {
    // Check if the requested CV is in the indexed access page (257-512)
    if (cv_number >= 257 && cv_number <= 512) {
        uint8_t cv31 = readCV(CV_INDEXED_CV_HIGH_BYTE);
        uint8_t cv32 = readCV(CV_INDEXED_CV_LOW_BYTE);

        // RCN-227 specifies CV31 must be 0 for this type of mapping
        if (cv31 == 0) {
            uint16_t page_offset = cv_number - 257;
            switch (cv32) {
                case 40: // RCN-227 "Per Function"
                    return RCN227_PF_BLOCK_CV_BASE + page_offset;
                case 41: // RCN-227 "Per Output" V1
                    return RCN227_PO_V1_BLOCK_CV_BASE + page_offset;
                case 42: // RCN-227 "Per Output" V2
                    return RCN227_PO_V2_BLOCK_CV_BASE + page_offset;
                case 43: // RCN-227 "Per Output" V3
                    return RCN227_PO_V3_BLOCK_CV_BASE + page_offset;
                default:
                    // Not a recognized RCN-227 page, treat as a direct access
                    break;
            }
        }
    }
    // If not in the indexed range or no mapping applies, return the original address
    return cv_number;
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

    // --- Motor Control (PID) ---
    _cv_values[CV_MOTOR_CONFIGURATION] = DECODER_DEFAULT_MOTOR_CONFIGURATION;
    _cv_values[CV_PID_KP] = DECODER_DEFAULT_PID_KP;
    _cv_values[CV_PID_KI] = DECODER_DEFAULT_PID_KI;

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
