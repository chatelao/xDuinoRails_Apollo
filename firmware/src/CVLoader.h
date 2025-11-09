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
    /**
     * @brief Parses the standard RCN-225 function mapping (CVs 33-46).
     */
    static void parseRcn225(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);

    /**
     * @brief Parses the RCN-227 "per function" mapping.
     */
    static void parseRcn227PerFunction(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);

    /**
     * @brief Parses the RCN-227 "per output" Version 1 (Matrix) mapping.
     */
    static void parseRcn227PerOutputV1(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);

    /**
     * @brief Parses the RCN-227 "per output" Version 2 (Function Number) mapping.
     */
    static void parseRcn227PerOutputV2(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);
};

#endif // CV_LOADER_H
