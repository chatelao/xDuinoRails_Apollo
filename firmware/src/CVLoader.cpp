#include "CVLoader.h"
#include "cv_definitions.h"
#include "CVManager.h"
#include "FunctionManager.h"
#include "Effect.h"
#include "PhysicalOutputManager.h"
#include "LogicalFunction.h"
#include "FunctionMapping.h"

void CVLoader::loadCvToFunctionManager(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    // Clear any existing configuration from a previous load
    functionManager.reset();

    auto mapping_method = static_cast<FunctionMappingMethod>(cvManager.readCV(CV_FUNCTION_MAPPING_METHOD));

    switch (mapping_method) {
        case FunctionMappingMethod::RCN_225:
            parseRcn225(cvManager, functionManager, physicalOutputManager);
            break;
        case FunctionMappingMethod::RCN_227_PER_FUNCTION:
            parseRcn227PerFunction(cvManager, functionManager, physicalOutputManager);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V1:
            parseRcn227PerOutputV1(cvManager, functionManager, physicalOutputManager);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V2:
            parseRcn227PerOutputV2(cvManager, functionManager, physicalOutputManager);
            break;
        case FunctionMappingMethod::PROPRIETARY:
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V3:
        default:
            // Do nothing for unsupported or proprietary mapping methods yet
            return;
    }
}

void CVLoader::parseRcn225(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
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
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 0}); // F0
        } else if (i == 1) { // CV 34: F0 Reverse
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 0}); // F0
        } else { // CVs 35-46: F1 - F12
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)(i - 1)}); // CV 35 (i=2) is F1
        }
        functionManager.addConditionVariable(cv);

        // For each bit set in the mask, create a LogicalFunction and a MappingRule
        for (int output_bit = 0; output_bit < 8; ++output_bit) {
            if ((mapping_mask >> output_bit) & 1) {
                uint8_t physical_output_id = output_bit + 1;

                Effect* effect = new EffectSteady(255);
                LogicalFunction* lf = new LogicalFunction(effect);
                lf->addOutput(physicalOutputManager.getOutputById(physical_output_id));
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

void CVLoader::parseRcn227PerFunction(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 40);

    const int num_functions = 32;

    for (int func_num = 0; func_num < num_functions; ++func_num) {
        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (func_num * 2 + dir) * 4;
            uint32_t output_mask = (uint32_t)cvManager.readCV(base_cv + 2) << 16 | (uint32_t)cvManager.readCV(base_cv + 1) << 8 | cvManager.readCV(base_cv);
            uint8_t blocking_func_num = cvManager.readCV(base_cv + 3);

            if (output_mask == 0) continue;

            ConditionVariable cv;
            cv.id = (func_num * 2) + dir + 1;
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
            functionManager.addConditionVariable(cv);

            uint8_t blocking_cv_id = 0;
            if (blocking_func_num != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 100 + blocking_func_num;
                blocking_cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func_num});
                functionManager.addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int output_bit = 0; output_bit < 24; ++output_bit) {
                if ((output_mask >> output_bit) & 1) {
                    uint8_t physical_output_id = output_bit + 1;
                    LogicalFunction* lf = new LogicalFunction(new EffectSteady(255));
                    lf->addOutput(physicalOutputManager.getOutputById(physical_output_id));
                    functionManager.addLogicalFunction(lf);
                    uint8_t lf_idx = functionManager.getLogicalFunctionCount() - 1;

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) rule.negative_conditions.push_back(blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    functionManager.addMappingRule(rule);
                }
            }
        }
    }
}

void CVLoader::parseRcn227PerOutputV1(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 41);

    const int num_outputs = 24;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr; // Lazily created

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint32_t func_mask = (uint32_t)cvManager.readCV(base_cv + 3) << 24 | (uint32_t)cvManager.readCV(base_cv + 2) << 16 | (uint32_t)cvManager.readCV(base_cv + 1) << 8 | cvManager.readCV(base_cv);

            if (func_mask == 0) continue;

            if (lf == nullptr) {
                lf = new LogicalFunction(new EffectSteady(255));
                lf->addOutput(physicalOutputManager.getOutputById(output_num + 1));
                functionManager.addLogicalFunction(lf);
            }
            uint8_t lf_idx = functionManager.getLogicalFunctionCount() - 1;

            for (int func_num = 0; func_num < 32; ++func_num) {
                if ((func_mask >> func_num) & 1) {
                    ConditionVariable cv;
                    cv.id = 200 + (output_num * 64) + (dir * 32) + func_num; // Unique ID
                    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
                    cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    functionManager.addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    rule.action = MappingAction::ACTIVATE;
                    functionManager.addMappingRule(rule);
                }
            }
        }
    }
}

void CVLoader::parseRcn227PerOutputV2(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 42);

    const int num_outputs = 32;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr;

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint8_t funcs[] = {
                cvManager.readCV(base_cv),
                cvManager.readCV(base_cv + 1),
                cvManager.readCV(base_cv + 2)
            };
            uint8_t blocking_func = cvManager.readCV(base_cv + 3);

            uint8_t blocking_cv_id = 0;
            if (blocking_func != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 400 + blocking_func; // Unique ID
                blocking_cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func});
                functionManager.addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int i = 0; i < 3; ++i) {
                if (funcs[i] != 255) {
                    if (lf == nullptr) {
                        lf = new LogicalFunction(new EffectSteady(255));
                        lf->addOutput(physicalOutputManager.getOutputById(output_num + 1));
                        functionManager.addLogicalFunction(lf);
                    }
                    uint8_t lf_idx = functionManager.getLogicalFunctionCount() - 1;

                    ConditionVariable cv;
                    cv.id = 500 + (output_num * 8) + (dir * 4) + i; // Unique ID
                    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, funcs[i]});
                    cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    functionManager.addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) rule.negative_conditions.push_back(blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    functionManager.addMappingRule(rule);
                }
            }
        }
    }
}
