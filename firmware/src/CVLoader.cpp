#include "CVLoader.h"
#include "cv_definitions.h"
#include "CVManager.h"
#include "FunctionManager.h"
#include "Effect.h"
#include "PhysicalOutputManager.h"
#include "LogicalFunction.h"

void CVLoader::loadCvToFunctionManager(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    // Clear any existing configuration from a previous load
    functionManager.reset();

    uint8_t mapping_method = cvManager.readCV(CV_FUNCTION_MAPPING_METHOD);

    // Only load the RCN-225 mapping if CV 96 is set to 1
    if (mapping_method != 1) {
        return;
    }

    // The RCN-225 mapping is a direct mapping from a function key (and direction) to one or more physical outputs.
    // To implement this within the existing data-driven system, we will auto-generate the necessary
    // LogicalFunctions, ConditionVariables, and MappingRules based on the CV values.

    // Each physical output that is mapped will get its own LogicalFunction.
    // Each function key activation (e.g., F1 ON, F0 FWD ON) will get its own ConditionVariable.
    // A MappingRule will then link the condition to the logical function.

    // CVs 33-46 correspond to F0_fwd, F0_rev, F1, F2, ..., F12
    const int num_mapping_cvs = CV_OUTPUT_LOCATION_CONFIG_END - CV_OUTPUT_LOCATION_CONFIG_START + 1;

    for (int i = 0; i < num_mapping_cvs; ++i) {
        uint16_t cv_addr = CV_OUTPUT_LOCATION_CONFIG_START + i;
        uint8_t mapping_mask = cvManager.readCV(cv_addr);

        if (mapping_mask == 0) {
            continue; // No outputs mapped for this function condition
        }

        // Create the ConditionVariable for this function trigger
        ConditionVariable cv;
        cv.id = i + 1; // Use a simple 1-based ID

        if (i == 0) { // CV 33: F0 Forward
            Condition dir_cond;
            dir_cond.source = TriggerSource::DIRECTION;
            dir_cond.comparator = TriggerComparator::IS_FWD;
            cv.conditions.push_back(dir_cond);

            Condition f0_cond;
            f0_cond.source = TriggerSource::FUNCTION_KEY;
            f0_cond.comparator = TriggerComparator::IS_ON;
            f0_cond.parameter = 0; // F0
            cv.conditions.push_back(f0_cond);

        } else if (i == 1) { // CV 34: F0 Reverse
            Condition dir_cond;
            dir_cond.source = TriggerSource::DIRECTION;
            dir_cond.comparator = TriggerComparator::IS_REV;
            cv.conditions.push_back(dir_cond);

            Condition f0_cond;
            f0_cond.source = TriggerSource::FUNCTION_KEY;
            f0_cond.comparator = TriggerComparator::IS_ON;
            f0_cond.parameter = 0; // F0
            cv.conditions.push_back(f0_cond);

        } else { // CVs 35-46: F1 - F12
            Condition c;
            c.source = TriggerSource::FUNCTION_KEY;
            c.comparator = TriggerComparator::IS_ON;
            c.parameter = i - 1; // CV 35 (i=2) is F1, CV 36 (i=3) is F2, etc.
            cv.conditions.push_back(c);
        }
        functionManager.addConditionVariable(cv);

        // For each bit set in the mask, create a LogicalFunction and a MappingRule
        for (int output_bit = 0; output_bit < 8; ++output_bit) {
            if ((mapping_mask >> output_bit) & 1) {
                uint8_t physical_output_id = output_bit + 1; // RCN mapping is 1-based (bit 0 = output 1)

                // Create a LogicalFunction for this specific mapping.
                // For this basic RCN-225 implementation, all outputs are simple 'steady on'.
                Effect* effect = new EffectSteady(255); // Full brightness
                LogicalFunction* lf = new LogicalFunction(effect);

                PhysicalOutput* output = physicalOutputManager.getOutputById(physical_output_id);
                if (output != nullptr) {
                    lf->addOutput(output);
                }
                functionManager.addLogicalFunction(lf);
                uint8_t lf_idx = functionManager.getLogicalFunctionCount() - 1; // Get the 0-based index

                // Create the MappingRule to link the ConditionVariable to the LogicalFunction
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                rule.positive_conditions.push_back(cv.id);
                rule.action = MappingAction::TURN_ON;
                functionManager.addMappingRule(rule);
            }
        }
    }
}
