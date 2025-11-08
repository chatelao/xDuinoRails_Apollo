#include "CVLoader.h"
#include "cv_definitions.h"
#include "CVManager.h"
#include "FunctionManager.h"
#include "Effect.h"
#include "PhysicalOutputManager.h"
#include "LogicalFunction.h"
#include "FunctionMapping.h"

// Forward declaration for the new parsing function
static void parseRcn227PerFunction(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager);

void CVLoader::loadCvToFunctionManager(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    // Clear any existing configuration from a previous load
    functionManager.reset();

    auto mapping_method = static_cast<FunctionMappingMethod>(cvManager.readCV(CV_FUNCTION_MAPPING_METHOD));

    switch (mapping_method) {
        case FunctionMappingMethod::RCN_225:
            // CVs 33-46 correspond to F0_fwd, F0_rev, F1, F2, ..., F12
            break;
        case FunctionMappingMethod::RCN_227_PER_FUNCTION:
            parseRcn227PerFunction(cvManager, functionManager, physicalOutputManager);
            return; // Return after parsing to avoid falling through
        case FunctionMappingMethod::PROPRIETARY:
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V1:
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V2:
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V3:
        default:
            // Do nothing for unsupported or proprietary mapping methods yet
            return;
    }
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

/**
 * @brief Parses the RCN-227 "per function" mapping CVs.
 * @details Implements the logic described in RCN-227, Section 2. It reads a block of
 *          CVs that map each function key (and direction) to a bitmask of physical
 *          outputs and an optional blocking function.
 */
static void parseRcn227PerFunction(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    // Set CVs 31 and 32 to select the correct page for RCN-227 "per function" mapping.
    // This ensures that subsequent reads of CVs 257-512 access the correct data.
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 40);

    const int num_functions = 32; // F0 to F31

    for (int func_num = 0; func_num < num_functions; ++func_num) {
        for (int dir = 0; dir < 2; ++dir) { // 0 for forward, 1 for reverse
            // Each function/direction pair has a 4-byte block.
            // The CVs start at 257.
            uint16_t base_cv = 257 + (func_num * 2 + dir) * 4;

            uint8_t mask_byte1 = cvManager.readCV(base_cv);
            uint8_t mask_byte2 = cvManager.readCV(base_cv + 1);
            uint8_t mask_byte3 = cvManager.readCV(base_cv + 2);
            uint8_t blocking_func_num = cvManager.readCV(base_cv + 3);

            uint32_t output_mask = (uint32_t)mask_byte3 << 16 | (uint32_t)mask_byte2 << 8 | mask_byte1;

            if (output_mask == 0) {
                continue; // No outputs are mapped for this function, so skip.
            }

            // --- Create the main Condition Variable for this trigger ---
            ConditionVariable cv;
            cv.id = (func_num * 2) + dir + 1; // Unique ID for this trigger

            Condition func_cond;
            func_cond.source = TriggerSource::FUNC_KEY;
            func_cond.comparator = TriggerComparator::IS_TRUE;
            func_cond.parameter = func_num;
            cv.conditions.push_back(func_cond);

            Condition dir_cond;
            dir_cond.source = TriggerSource::DIRECTION;
            dir_cond.comparator = TriggerComparator::EQ;
            dir_cond.parameter = (dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE;
            cv.conditions.push_back(dir_cond);
            functionManager.addConditionVariable(cv);

            // --- Create a separate Condition Variable for the blocking function if it exists ---
            uint8_t blocking_cv_id = 0;
            if (blocking_func_num != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 100 + blocking_func_num; // Use a high offset for blocking CV IDs
                Condition block_cond;
                block_cond.source = TriggerSource::FUNC_KEY;
                block_cond.comparator = TriggerComparator::IS_TRUE;
                block_cond.parameter = blocking_func_num;
                blocking_cv.conditions.push_back(block_cond);
                functionManager.addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            // --- Create Logical Functions and Mapping Rules for each active output ---
            for (int output_bit = 0; output_bit < 24; ++output_bit) {
                if ((output_mask >> output_bit) & 1) {
                    uint8_t physical_output_id = output_bit + 1; // Outputs are 1-based

                    // Create a simple "ON" effect for this output
                    Effect* effect = new EffectSteady(255);
                    LogicalFunction* lf = new LogicalFunction(effect);

                    PhysicalOutput* output = physicalOutputManager.getOutputById(physical_output_id);
                    if (output != nullptr) {
                        lf->addOutput(output);
                    }
                    functionManager.addLogicalFunction(lf);
                    uint8_t lf_idx = functionManager.getLogicalFunctionCount() - 1;

                    // Create the rule to activate this logical function
                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) {
                        rule.negative_conditions.push_back(blocking_cv_id);
                    }
                    rule.action = MappingAction::ACTIVATE;
                    functionManager.addMappingRule(rule);
                }
            }
        }
    }
}
