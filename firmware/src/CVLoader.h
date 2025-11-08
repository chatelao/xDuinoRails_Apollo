#ifndef CV_LOADER_H
#define CV_LOADER_H

// Forward declarations
class CVManager;
class FunctionManager;
class PhysicalOutputManager;

/**
 * @file CVLoader.h
 * @brief Handles loading configuration from CVs into the FunctionManager.
 */

class CVLoader {
public:
    /**
     * @brief Reads all relevant CVs and populates the FunctionManager.
     * @param cvManager The CVManager instance to read from.
     * @param functionManager The FunctionManager instance to populate.
     * @param physicalOutputManager The PhysicalOutputManager for linking outputs.
     */
    static void loadCvToFunctionManager(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);

private:
    static void loadLogicalFunctions(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);
    static void loadConditionVariables(CVManager& cvManager, FunctionManager& functionManager);
    static void loadMappingRules(CVManager& cvManager, FunctionManager& functionManager);
};

#endif // CV_LOADER_H
