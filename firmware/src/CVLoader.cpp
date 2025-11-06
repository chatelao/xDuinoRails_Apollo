#include "CVLoader.h"
#include "CVManager.h"
#include "FunctionManager.h"
#include "LightEffect.h"
#include "PhysicalOutputManager.h"
#include "LogicalFunction.h"

// Define the size of each configuration block
#define MAX_LOGICAL_FUNCTIONS 32
#define MAX_CONDITION_VARIABLES 32
#define MAX_MAPPING_RULES 64

#define CV_LOGICAL_FUNCTION_SIZE 8
#define CV_CONDITION_VARIABLE_SIZE 4
#define CV_MAPPING_RULE_SIZE 4

void CVLoader::loadCvToFunctionManager(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    loadLogicalFunctions(cvManager, functionManager, physicalOutputManager);
    loadConditionVariables(cvManager, functionManager);
    loadMappingRules(cvManager, functionManager);
}

void CVLoader::loadLogicalFunctions(CVManager& cvManager, FunctionManager& functionManager, PhysicalOutputManager& physicalOutputManager) {
    for (int i = 0; i < MAX_LOGICAL_FUNCTIONS; ++i) {
        uint16_t base_cv = CV_BASE_LOGICAL_FUNCTIONS + (i * CV_LOGICAL_FUNCTION_SIZE);
        uint8_t effect_type = cvManager.readCV(base_cv + 0);

        if (effect_type == 0) continue; // Type 0 means unused / end of list

        uint8_t param1 = cvManager.readCV(base_cv + 1);
        uint8_t param2 = cvManager.readCV(base_cv + 2);
        uint8_t param3 = cvManager.readCV(base_cv + 3);
        uint8_t physical_output_id = cvManager.readCV(base_cv + 4);

        LightEffect* effect = nullptr;
        switch (effect_type) {
            case 1: effect = new EffectSteady(param1); break;
            case 2: effect = new EffectDimming(param1, param2); break;
            case 3: effect = new EffectFlicker(param1, param2, param3); break;
            case 4: effect = new EffectStrobe(param1, param2, param3); break;
            case 5: effect = new EffectMarsLight(param1, param2, param3); break;
            case 6: effect = new EffectSoftStartStop(param1, param2, param3); break;
            default: effect = new EffectSteady(0); break;
        }

        PhysicalOutput* output = physicalOutputManager.getOutputById(physical_output_id);
        LogicalFunction* lf = new LogicalFunction(effect);
        if (output != nullptr) {
            lf->addOutput(output);
        }
        functionManager.addLogicalFunction(lf);
    }
}

void CVLoader::loadConditionVariables(CVManager& cvManager, FunctionManager& functionManager) {
    for (int i = 0; i < MAX_CONDITION_VARIABLES; ++i) {
        uint16_t base_cv = CV_BASE_COND_VARS + (i * CV_CONDITION_VARIABLE_SIZE);
        uint8_t source_type = cvManager.readCV(base_cv + 0);

        if (source_type == 0) continue; // Type 0 means unused / end of list

        ConditionVariable cv;
        cv.id = i + 1; // IDs are 1-based
        Condition c;
        c.source = (TriggerSource)source_type;
        c.comparator = (TriggerComparator)cvManager.readCV(base_cv + 1);
        c.parameter = cvManager.readCV(base_cv + 2);
        cv.conditions.push_back(c);
        functionManager.addConditionVariable(cv);
    }
}

void CVLoader::loadMappingRules(CVManager& cvManager, FunctionManager& functionManager) {
    for (int i = 0; i < MAX_MAPPING_RULES; ++i) {
        uint16_t base_cv = CV_BASE_MAPPING_RULES + (i * CV_MAPPING_RULE_SIZE);
        uint8_t target_lf_id = cvManager.readCV(base_cv + 0);

        if (target_lf_id == 0) continue; // Target 0 means unused / end of list

        MappingRule rule;
        rule.target_logical_function_id = target_lf_id - 1; // CV is 1-based, index is 0-based
        rule.positive_conditions.push_back(cvManager.readCV(base_cv + 1));
        rule.negative_conditions.push_back(cvManager.readCV(base_cv + 2));
        rule.action = (MappingAction)cvManager.readCV(base_cv + 3);
        functionManager.addMappingRule(rule);
    }
}
