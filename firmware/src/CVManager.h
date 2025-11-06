#ifndef CV_MANAGER_H
#define CV_MANAGER_H

#include <map>
#include <cstdint>

/**
 * @file CVManager.h
 * @brief Manages decoder Configuration Variables (CVs).
 */

// --- CV Address Definitions ---

// Base addresses for configuration blocks. Each block is sized to allow
// for multiple instances of the configured item (e.g., multiple logical funcs).
#define CV_BASE_LOGICAL_FUNCTIONS  200 // Block for Logical Function configs (32 funcs * 8 CVs/func = 256 CVs, oversized for safety)
#define CV_BASE_COND_VARS          500 // Block for Condition Variables (32 CVs * 4 CVs/var = 128 CVs)
#define CV_BASE_MAPPING_RULES      700 // Block for Mapping Rules (64 rules * 4 CVs/rule = 256 CVs)

// --- CVManager Class ---

class CVManager {
public:
    CVManager();

    /**
     * @brief Reads a CV value.
     * @param cv_number The CV number (1-1024).
     * @return The value of the CV. Returns 0 if the CV is not found.
     */
    uint8_t readCV(uint16_t cv_number);

    /**
     * @brief Writes a value to a CV.
     * @param cv_number The CV number (1-1024).
     * @param value The value to write.
     */
    void writeCV(uint16_t cv_number, uint8_t value);

    /**
     * @brief Loads the CVs from EEPROM (or sets defaults).
     *        This should be called at startup.
     */
    void begin();

private:
    void setDefaultCVs();
    void loadCVsFromEeprom();
    void writeCvToEeprom(uint16_t cv_number, uint8_t value);

    std::map<uint16_t, uint8_t> _cv_values;
};

#endif // CV_MANAGER_H
