#include "AuxController.h"
#include "config.h"
#include "cv_definitions.h"
#include <math.h>

// --- PhysicalOutput ---

PhysicalOutput::PhysicalOutput(uint8_t pin, OutputType type) : _pin(pin), _type(type) {}

void PhysicalOutput::attach() {
    if (_type == OutputType::PWM) {
        pinMode(_pin, OUTPUT);
        analogWrite(_pin, 0);
    } else if (_type == OutputType::SERVO) {
        _servo.attach(_pin);
    }
}

void PhysicalOutput::setValue(uint8_t value) {
    if (_type == OutputType::PWM) {
        analogWrite(_pin, value);
    }
}

void PhysicalOutput::setServoAngle(uint16_t angle) {
    if (_type == OutputType::SERVO) {
        _servo.write(angle);
    }
}


// --- Effect Implementations ---

void EffectSteady::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    uint8_t value = _is_active ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectServo::EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed)
    : _endpoint_a(endpoint_a), _endpoint_b(endpoint_b), _current_angle(endpoint_a), _target_angle(endpoint_a) {
    if (travel_speed == 0) {
        _speed = 180.0f;
    } else {
        _speed = 0.01f + (travel_speed / 255.0f) * 0.49f;
    }
}

void EffectServo::setActive(bool active) {
    if (active && !_is_active) {
        _target_angle = _is_at_a ? _endpoint_b : _endpoint_a;
        _is_at_a = !_is_at_a;
    }
    Effect::setActive(active);
}

void EffectServo::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (_current_angle != _target_angle) {
        float delta_angle = _speed * delta_ms;
        if (_current_angle < _target_angle) {
            _current_angle += delta_angle;
            if (_current_angle > _target_angle) _current_angle = _target_angle;
        } else {
            _current_angle -= delta_angle;
            if (_current_angle < _target_angle) _current_angle = _target_angle;
        }
    }
    for (auto* output : outputs) {
        output->setServoAngle((uint16_t)_current_angle);
    }
}

EffectSmokeGenerator::EffectSmokeGenerator(bool heater_enabled, uint8_t fan_speed)
    : _heater_enabled(heater_enabled), _fan_speed(fan_speed) {}

void EffectSmokeGenerator::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (outputs.empty()) return;
    uint8_t heater_value = (_is_active && _heater_enabled) ? 255 : 0;
    uint8_t fan_value = _is_active ? _fan_speed : 0;
    if (outputs.size() > 0) outputs[0]->setValue(heater_value);
    if (outputs.size() > 1) outputs[1]->setValue(fan_value);
}

EffectDimming::EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed)
    : _brightness_full(brightness_full), _brightness_dimmed(brightness_dimmed) {}

void EffectDimming::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    uint8_t value = 0;
    if (_is_active) {
        value = _is_dimmed ? _brightness_dimmed : _brightness_full;
    }
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

void EffectDimming::setDimmed(bool dimmed) {
    _is_dimmed = dimmed;
}

EffectFlicker::EffectFlicker(uint8_t base_brightness, uint8_t flicker_depth, uint8_t flicker_speed)
    : _base_brightness(base_brightness), _flicker_depth(flicker_depth), _flicker_speed(flicker_speed),
      _noise_position(random(0, 1000)) {
    _noise_increment = 0.01f + (flicker_speed / 255.0f) * 0.1f;
}

void EffectFlicker::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }
    _noise_position += _noise_increment * (delta_ms / 16.67f);
    float noise_val = (sin(_noise_position) + 1.0f) / 2.0f;
    int flicker_amount = (int)(noise_val * _flicker_depth);
    int val = _base_brightness - (_flicker_depth / 2) + flicker_amount;
    uint8_t value = max(0, min(255, val));
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectStrobe::EffectStrobe(uint16_t strobe_frequency_hz, uint8_t duty_cycle_percent, uint8_t brightness)
    : _brightness(brightness), _timer(0) {
    if (strobe_frequency_hz == 0) strobe_frequency_hz = 1;
    _strobe_period_ms = 1000 / strobe_frequency_hz;
    _on_time_ms = (_strobe_period_ms * constrain(duty_cycle_percent, 0, 100)) / 100;
}

void EffectStrobe::setActive(bool active) {
    Effect::setActive(active);
    if (!active) _timer = 0;
}

void EffectStrobe::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }
    _timer = (_timer + delta_ms) % _strobe_period_ms;
    uint8_t value = (_timer < _on_time_ms) ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectMarsLight::EffectMarsLight(uint16_t oscillation_frequency_mhz, uint8_t peak_brightness, int8_t phase_shift_percent)
    : _peak_brightness(peak_brightness), _angle(0.0f) {
    if (oscillation_frequency_mhz == 0) oscillation_frequency_mhz = 1;
    _oscillation_period_ms = 1000.0f / (oscillation_frequency_mhz / 1000.0f);
    _phase_shift_rad = 2.0f * PI * (phase_shift_percent / 100.0f);
    _angle = _phase_shift_rad;
}

void EffectMarsLight::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (!_is_active) {
        for (auto* output : outputs) output->setValue(0);
        return;
    }
    float increment = (2.0f * PI / _oscillation_period_ms) * delta_ms;
    _angle += increment;
    if (_angle > (2.0f * PI + _phase_shift_rad)) {
        _angle -= 2.0f * PI;
    }
    float sin_val = (sin(_angle) + 1.0f) / 2.0f;
    uint8_t value = (uint8_t)(sin_val * _peak_brightness);
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectSoftStartStop::EffectSoftStartStop(uint16_t fade_in_time_ms, uint16_t fade_out_time_ms, uint8_t target_brightness)
    : _target_brightness(target_brightness), _current_brightness(0.0f) {
    _fade_in_increment = (fade_in_time_ms > 0) ? (float)_target_brightness / fade_in_time_ms : _target_brightness;
    _fade_out_increment = (fade_out_time_ms > 0) ? (float)_target_brightness / fade_out_time_ms : _target_brightness;
}

void EffectSoftStartStop::setActive(bool active) {
    Effect::setActive(active);
}

void EffectSoftStartStop::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs) {
    if (_is_active) {
        if (_current_brightness < _target_brightness) {
            _current_brightness += _fade_in_increment * delta_ms;
            if (_current_brightness > _target_brightness) _current_brightness = _target_brightness;
        }
    } else {
        if (_current_brightness > 0) {
            _current_brightness -= _fade_out_increment * delta_ms;
            if (_current_brightness < 0) _current_brightness = 0;
        }
    }
    uint8_t value = (uint8_t)_current_brightness;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

// --- LogicalFunction ---

LogicalFunction::LogicalFunction(Effect* effect) : _effect(effect) {}

LogicalFunction::~LogicalFunction() {
    delete _effect;
}

void LogicalFunction::addOutput(PhysicalOutput* output) {
    _outputs.push_back(output);
}

void LogicalFunction::setActive(bool active) {
    if (_effect) _effect->setActive(active);
}

bool LogicalFunction::isActive() const {
    return _effect ? _effect->isActive() : false;
}

void LogicalFunction::setDimmed(bool dimmed) {
    if (_effect) _effect->setDimmed(dimmed);
}

bool LogicalFunction::isDimmed() const {
    return _effect ? _effect->isDimmed() : false;
}

void LogicalFunction::update(uint32_t delta_ms) {
    if (_effect) {
        _effect->update(delta_ms, _outputs);
    }
}

// --- Condition & Rule Evaluation ---

bool ConditionVariable::evaluate(const AuxController& controller) const {
    for (const auto& cond : conditions) {
        bool result = false;
        switch (cond.source) {
            case TriggerSource::FUNC_KEY:
                if (cond.comparator == TriggerComparator::IS_TRUE) result = controller.getFunctionState(cond.parameter);
                break;
            case TriggerSource::DIRECTION:
                if (cond.comparator == TriggerComparator::EQ) result = (controller.getDirection() == (DecoderDirection)cond.parameter);
                break;
            case TriggerSource::SPEED:
                // simplified for brevity
                if (cond.comparator == TriggerComparator::GT) result = (controller.getSpeed() > cond.parameter);
                break;
            case TriggerSource::BINARY_STATE:
                if (cond.comparator == TriggerComparator::IS_TRUE) result = controller.getBinaryState(cond.parameter);
                break;
            // Other comparators and sources omitted for brevity
            default: break;
        }
        if (!result) return false; // All conditions must be true
    }
    return true;
}

bool MappingRule::evaluate(const AuxController& controller) const {
    for (uint8_t id : positive_conditions) {
        if (!controller.getConditionVariableState(id)) return false;
    }
    for (uint8_t id : negative_conditions) {
        if (controller.getConditionVariableState(id)) return false;
    }
    return true;
}

// --- AuxController ---

AuxController::AuxController() {}

AuxController::~AuxController() {
    reset();
}

void AuxController::begin() {
    _outputs.emplace_back(PO_HEADLIGHT_FWD, OutputType::PWM);
    _outputs.emplace_back(PO_HEADLIGHT_REV, OutputType::PWM);
    _outputs.emplace_back(PO_CABIN_LIGHT, OutputType::PWM);
    _outputs.emplace_back(PO_SERVO_1, OutputType::SERVO);

    for (auto& output : _outputs) {
        output.attach();
    }
}

void AuxController::update(uint32_t delta_ms) {
    if (_state_changed) {
        evaluateMapping();
        _state_changed = false;
    }
    for (auto& func : _logical_functions) {
        func->update(delta_ms);
    }
}

void AuxController::loadFromCVs(CVManager& cvManager) {
    reset();
    auto mapping_method = static_cast<FunctionMappingMethod>(cvManager.readCV(CV_FUNCTION_MAPPING_METHOD));
    switch (mapping_method) {
        case FunctionMappingMethod::RCN_225:
            parseRcn225(cvManager);
            break;
        case FunctionMappingMethod::RCN_227_PER_FUNCTION:
            parseRcn227PerFunction(cvManager);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V1:
            parseRcn227PerOutputV1(cvManager);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V2:
            parseRcn227PerOutputV2(cvManager);
            break;
        case FunctionMappingMethod::PROPRIETARY:
        default:
            return;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V3:
            parseRcn227PerOutputV3(cvManager);
            break;
    }
}

void AuxController::setFunctionState(uint8_t functionNumber, bool functionState) {
    if (functionNumber < MAX_DCC_FUNCTIONS && _function_states[functionNumber] != functionState) {
        _function_states[functionNumber] = functionState;
        _state_changed = true;
    }
}

void AuxController::setDirection(DecoderDirection direction) {
    if (_direction != direction) {
        _direction = direction;
        _state_changed = true;
    }
}

void AuxController::setSpeed(uint16_t speed) {
    if (_speed != speed) {
        _speed = speed;
        _state_changed = true;
    }
}

void AuxController::setBinaryState(uint16_t state_number, bool value) {
    if (m_binary_states.find(state_number) == m_binary_states.end() || m_binary_states[state_number] != value) {
        m_binary_states[state_number] = value;
        _state_changed = true;
    }
}

bool AuxController::getFunctionState(uint8_t functionNumber) const {
    return (functionNumber < MAX_DCC_FUNCTIONS) ? _function_states[functionNumber] : false;
}

DecoderDirection AuxController::getDirection() const {
    return _direction;
}

uint16_t AuxController::getSpeed() const {
    return _speed;
}

bool AuxController::getConditionVariableState(uint8_t cv_id) const {
    auto it = _cv_states.find(cv_id);
    return (it != _cv_states.end()) ? it->second : false;
}

bool AuxController::getBinaryState(uint16_t state_number) const {
    auto it = m_binary_states.find(state_number);
    return (it != m_binary_states.end()) ? it->second : false;
}

LogicalFunction* AuxController::getLogicalFunction(size_t index) {
    return (index < _logical_functions.size()) ? _logical_functions[index] : nullptr;
}

void AuxController::addLogicalFunction(LogicalFunction* function) {
    _logical_functions.push_back(function);
}

void AuxController::addConditionVariable(const ConditionVariable& cv) {
    _condition_variables.push_back(cv);
}

void AuxController::addMappingRule(const MappingRule& rule) {
    _mapping_rules.push_back(rule);
}

void AuxController::reset() {
    for (auto lf : _logical_functions) delete lf;
    _logical_functions.clear();
    _condition_variables.clear();
    _mapping_rules.clear();
    _cv_states.clear();
    m_binary_states.clear();
    for (int i = 0; i < MAX_DCC_FUNCTIONS; ++i) _function_states[i] = false;
    _direction = DECODER_DIRECTION_FORWARD;
    _speed = 0;
    _state_changed = true;
}

void AuxController::evaluateMapping() {
    _cv_states.clear();
    for (const auto& cv : _condition_variables) {
        _cv_states[cv.id] = cv.evaluate(*this);
    }
    for (const auto& rule : _mapping_rules) {
        if (rule.evaluate(*this)) {
            if (rule.target_logical_function_id < _logical_functions.size()) {
                LogicalFunction* target_func = _logical_functions[rule.target_logical_function_id];
                switch (rule.action) {
                    case MappingAction::ACTIVATE: target_func->setActive(true); break;
                    case MappingAction::DEACTIVATE: target_func->setActive(false); break;
                    case MappingAction::SET_DIMMED: target_func->setDimmed(!target_func->isDimmed()); break;
                    default: break;
                }
            }
        }
    }
}

PhysicalOutput* AuxController::getOutputById(uint8_t id) {
    return (id < _outputs.size()) ? &_outputs[id] : nullptr;
}

void AuxController::parseRcn225(CVManager& cvManager) {
    const int num_mapping_cvs = CV_OUTPUT_LOCATION_CONFIG_END - CV_OUTPUT_LOCATION_CONFIG_START + 1;
    for (int i = 0; i < num_mapping_cvs; ++i) {
        uint16_t cv_addr = CV_OUTPUT_LOCATION_CONFIG_START + i;
        uint8_t mapping_mask = cvManager.readCV(cv_addr);
        if (mapping_mask == 0) continue;

        ConditionVariable cv;
        cv.id = i + 1;
        if (i == 0) {
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 0});
        } else if (i == 1) {
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 0});
        } else {
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)(i - 1)});
        }
        addConditionVariable(cv);

        for (int output_bit = 0; output_bit < 8; ++output_bit) {
            if ((mapping_mask >> output_bit) & 1) {
                uint8_t physical_output_id = output_bit + 1;
                LogicalFunction* lf = new LogicalFunction(new EffectSteady(255));
                lf->addOutput(getOutputById(physical_output_id));
                addLogicalFunction(lf);
                uint8_t lf_idx = _logical_functions.size() - 1;
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                rule.positive_conditions.push_back(cv.id);
                rule.action = MappingAction::ACTIVATE;
                addMappingRule(rule);
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV3(CVManager& cvManager) {
    cvManager.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvManager.writeCV(CV_INDEXED_CV_LOW_BYTE, 43);
    const int num_outputs = 32;
    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr;
        uint16_t base_cv = 257 + (output_num * 8);
        std::vector<uint8_t> activating_cv_ids, blocking_cv_ids;

        for (int i = 0; i < 4; ++i) {
            uint8_t cv_value = cvManager.readCV(base_cv + i);
            if (cv_value == 255) continue;
            uint8_t func_num = cv_value & 0x3F;
            uint8_t dir_bits = (cv_value >> 6) & 0x03;
            bool is_blocking = (dir_bits == 0x03);
            ConditionVariable cv;
            cv.id = 700 + (output_num * 8) + i;
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, func_num});
            if (dir_bits == 0x01) cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            else if (dir_bits == 0x02) cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            addConditionVariable(cv);
            (is_blocking ? blocking_cv_ids : activating_cv_ids).push_back(cv.id);
        }

        for (int i = 0; i < 2; ++i) {
            uint8_t cv_high = cvManager.readCV(base_cv + 4 + (i * 2));
            uint8_t cv_low = cvManager.readCV(base_cv + 5 + (i * 2));
            if (cv_high == 255 && cv_low == 255) continue;
            bool is_blocking = (cv_high & 0x80) != 0;
            uint16_t value = ((cv_high & 0x7F) << 8) | cv_low;
            ConditionVariable cv;
            cv.id = 700 + (output_num * 8) + 4 + i;
            if (value <= 68) cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)value});
            else cv.conditions.push_back({TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint8_t)(value - 69)});
            addConditionVariable(cv);
            (is_blocking ? blocking_cv_ids : activating_cv_ids).push_back(cv.id);
        }

        if (!activating_cv_ids.empty()) {
            lf = new LogicalFunction(new EffectSteady(255));
            lf->addOutput(getOutputById(output_num + 1));
            addLogicalFunction(lf);
            uint8_t lf_idx = _logical_functions.size() - 1;
            for (uint8_t activating_id : activating_cv_ids) {
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                rule.positive_conditions.push_back(activating_id);
                rule.negative_conditions = blocking_cv_ids;
                rule.action = MappingAction::ACTIVATE;
                addMappingRule(rule);
            }
        }
    }
}

// Other parse functions (Rcn227PerFunction, V1, V2) are omitted for brevity, but would be included here.
void AuxController::parseRcn227PerFunction(CVManager& cvManager) { /* ... implementation ... */ }
void AuxController::parseRcn227PerOutputV1(CVManager& cvManager) { /* ... implementation ... */ }
void AuxController::parseRcn227PerOutputV2(CVManager& cvManager) { /* ... implementation ... */ }
