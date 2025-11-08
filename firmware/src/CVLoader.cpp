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
            dir_cond.comparator = TriggerComparator::EQ;
            dir_cond.parameter = DECODER_DIRECTION_FORWARD;
            cv.conditions.push_back(dir_cond);

            Condition f0_cond;
            f0_cond.source = TriggerSource::FUNC_KEY;
            f0_cond.comparator = TriggerComparator::IS_TRUE;
            f0_cond.parameter = 0; // F0
            cv.conditions.push_back(f0_cond);

        } else if (i == 1) { // CV 34: F0 Reverse
            Condition dir_cond;
            dir_cond.source = TriggerSource::DIRECTION;
            dir_cond.comparator = TriggerComparator::EQ;
            dir_cond.parameter = DECODER_DIRECTION_REVERSE;
            cv.conditions.push_back(dir_cond);

            Condition f0_cond;
            f0_cond.source = TriggerSource::FUNC_KEY;
            f0_cond.comparator = TriggerComparator::IS_TRUE;
            f0_cond.parameter = 0; // F0
            cv.conditions.push_back(f0_cond);

        } else { // CVs 35-46: F1 - F12
            Condition c;
            c.source = TriggerSource::FUNC_KEY;
            c.comparator = TriggerComparator::IS_TRUE;
            c.parameter = i - 1; // CV 35 (i=2) is F1, CV 36 (i=3) is F2, etc.
            cv.conditions.push_back(c);
        }
        functionManager.addConditionVariable(cv);

        // For each bit set in the mask, create a LogicalFunction and a MappingRule
        for (int output_bit = 0; output_bit < 8; ++output_bit) {
            if ((mapping_mask >> output_bit) & 1) {
                uint8_t physical_output_id = output_bit + 1; // RCN mapping is 1-based (bit 0 = output 1)

                Effect* effect = new EffectSteady(255);
                LogicalFunction* lf = new LogicalFunction(effect);

                PhysicalOutput* output = physicalOutputManager.getOutputById(physical_output_id);
                if (output != nullptr) {
                    lf->addOutput(output);
                }
                functionManager.addLogicalFunction(lf);
                uint8_t lf_idx = functionManager.getLogicalFunctionCount() - 1;

                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                rule.positive_conditions.push_back(cv.id);
                rule.action = MappingAction::ACTIVATE;
                functionManager.addMappingRule(rule);
            }
        }
    }
}
