#ifndef CV_LOADER_H
#define CV_LOADER_H

class CVManager;
class FunctionManager;
class PhysicalOutputManager;

/**
 * @file CVLoader.h
 * @brief Populates the FunctionManager based on settings from the CVManager.
 */

class CVLoader {
public:
    /**
     * @brief Loads the configuration from CVs into the FunctionManager.
     * @param cvManager The source of the CV values.
     * @param functionManager The target for the created functions, rules, etc.
     * @param physicalOutputManager The source for physical output objects.
     */
    static void loadCvToFunctionManager(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);

private:
    static void loadLogicalFunctions(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);
    static void loadConditionVariables(CVManager& cvManager, FunctionManager& functionManager);
    static void loadMappingRules(CVManager& cvManager, FunctionManager& functionManager);
};

#endif // CV_LOADER_H
