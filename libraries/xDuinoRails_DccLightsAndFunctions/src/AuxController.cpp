/**
 * @file AuxController.cpp
 * @brief Implements the AuxController class and all related helper classes.
 */
#include "AuxController.h"
#include "cv_definitions.h"
#include <math.h>

namespace xDuinoRails {

// --- PhysicalOutput ---

PhysicalOutput::PhysicalOutput(uint8_t pin, OutputType type, uint16_t pwm_frequency, BrightnessCurve curve)
    : _pin(pin), _type(type), _pwm_frequency(pwm_frequency), _curve(curve) {}

void PhysicalOutput::attach() {
    switch (_type) {
        case OutputType::PWM_LOW_SIDE:
        case OutputType::PWM_HIGH_SIDE:
            pinMode(_pin, OUTPUT);
            // Note: Setting PWM frequency is board-specific. For RP2040, this can be
            // done with analogWriteFreq(_pwm_frequency); but is not standard Arduino API.
            // This implementation will use the default frequency.
            analogWrite(_pin, 0);
            break;
        case OutputType::ON_OFF:
            pinMode(_pin, OUTPUT);
            digitalWrite(_pin, LOW);
            break;
        case OutputType::SERVO:
            _servo.attach(_pin);
            break;
    }
}

uint8_t PhysicalOutput::applyCurve(uint8_t value) {
    if (_curve == BrightnessCurve::LINEAR || value == 0 || value == 255) {
        return value;
    }
    // Using floating point math for curve calculations.
    float normalized_value = value / 255.0f;
    float curved_value;
    switch (_curve) {
        case BrightnessCurve::LOGARITHMIC:
            // A simple logarithmic curve: y = log(x*15+1) / log(16)
            curved_value = log(normalized_value * 15.0f + 1.0f) / log(16.0f);
            break;
        case BrightnessCurve::EXPONENTIAL:
            // A simple exponential curve: y = x^2
            curved_value = normalized_value * normalized_value;
            break;
        default:
            curved_value = normalized_value;
            break;
    }
    return (uint8_t)(constrain(curved_value, 0.0f, 1.0f) * 255.0f);
}

void PhysicalOutput::setValue(uint8_t value) {
    uint8_t final_value = applyCurve(value);
    switch (_type) {
        case OutputType::PWM_LOW_SIDE:
            analogWrite(_pin, final_value);
            break;
        case OutputType::PWM_HIGH_SIDE:
            analogWrite(_pin, 255 - final_value);
            break;
        case OutputType::ON_OFF:
            digitalWrite(_pin, (final_value > 127) ? HIGH : LOW);
            break;
        case OutputType::SERVO:
            // This method is not intended for servos.
            break;
    }
}

void PhysicalOutput::setServoAngle(uint16_t angle) {
    if (_type == OutputType::SERVO) {
        _servo.write(angle);
    }
}


// --- Effect Implementations ---

void EffectSteady::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
    uint8_t value = _is_active ? _brightness : 0;
    for (auto* output : outputs) {
        output->setValue(value);
    }
}

EffectServo::EffectServo(uint8_t endpoint_a, uint8_t endpoint_b, uint8_t travel_speed, ServoMode mode)
    : _endpoint_a(endpoint_a), _endpoint_b(endpoint_b), _mode(mode), _current_angle(endpoint_a), _target_angle(endpoint_a) {
    if (travel_speed == 0) {
        _speed = 180.0f; // Effectively instant travel
    } else {
        // travel_speed is degrees per second, map from 1-255 to a reasonable range
        _speed = ((travel_speed / 255.0f) * 350.0f) + 10.0f; // degrees per second
    }
}

void EffectServo::setActive(bool active) {
    if (active != _is_active) {
        if (_mode == ServoMode::LATCHING) {
            if (active) {
                _target_angle = _is_at_a ? _endpoint_b : _endpoint_a;
                _is_at_a = !_is_at_a;
            }
        } else { // MOMENTARY
            _target_angle = active ? _endpoint_b : _endpoint_a;
        }
    }
    Effect::setActive(active);
}

void EffectServo::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
    if (_current_angle != _target_angle) {
        float delta_angle = _speed * (delta_ms / 1000.0f);
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

EffectSmokeGenerator::EffectSmokeGenerator(bool heater_enabled, SmokeFanMode fan_mode, uint8_t static_fan_speed, uint8_t max_fan_speed)
    : _heater_enabled(heater_enabled), _fan_mode(fan_mode), _static_fan_speed(static_fan_speed), _max_fan_speed(max_fan_speed) {}

void EffectSmokeGenerator::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
    if (outputs.empty()) return;

    uint8_t heater_value = (_is_active && _heater_enabled) ? 255 : 0;
    uint8_t fan_value = 0;

    if (_is_active) {
        if (_fan_mode == SmokeFanMode::STATIC) {
            fan_value = _static_fan_speed;
        } else { // SPEED_SYNC
            // Assuming max speed step is around 28 (common for DCC)
            float speed_ratio = constrain(controller.getSpeed() / 28.0f, 0.0f, 1.0f);
            fan_value = (uint8_t)(speed_ratio * _max_fan_speed);
        }
    }

    if (outputs.size() > 0) outputs[0]->setValue(heater_value);
    if (outputs.size() > 1) outputs[1]->setValue(fan_value);
}

EffectDimming::EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed)
    : _brightness_full(brightness_full), _brightness_dimmed(brightness_dimmed) {}

void EffectDimming::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
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

void EffectFlicker::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
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

void EffectStrobe::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
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

void EffectMarsLight::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
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

void EffectSoftStartStop::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
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

EffectNeonTube::EffectNeonTube(uint16_t warmup_time_ms, uint8_t steady_brightness)
    : _warmup_time_ms(warmup_time_ms), _steady_brightness(steady_brightness), _timer(0), _flicker_timer(0) {}

void EffectNeonTube::setActive(bool active) {
    Effect::setActive(active);
    if (active) {
        if (_state == State::OFF) {
            _state = State::WARMING_UP;
            _timer = 0;
        }
    } else {
        _state = State::OFF;
    }
}

void EffectNeonTube::update(uint32_t delta_ms, const std::vector<PhysicalOutput*>& outputs, const AuxController& controller) {
    uint8_t value = 0;
    _timer += delta_ms;

    switch (_state) {
        case State::OFF:
            value = 0;
            break;
        case State::WARMING_UP:
            // High frequency flicker during warm-up
            if ((_timer / 50) % 2 == 0) {
                value = _steady_brightness;
            } else {
                value = _steady_brightness / 2;
            }
            if (_timer >= _warmup_time_ms) {
                _state = State::STEADY;
                _flicker_timer = millis();
            }
            break;
        case State::STEADY:
            value = _steady_brightness;
            // Randomly decide to enter a flicker state
            if (millis() - _flicker_timer > 5000 && random(0, 100) < 5) {
                _state = State::FLICKERING;
                _timer = 0;
            }
            break;
        case State::FLICKERING:
            // A short, random flicker
            if ((_timer / 75) % 2 == 0) {
                value = _steady_brightness / 1.5;
            } else {
                value = _steady_brightness;
            }
            if (_timer > random(100, 300)) {
                _state = State::STEADY;
                _flicker_timer = millis();
            }
            break;
    }

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

void LogicalFunction::update(uint32_t delta_ms, const AuxController& controller) {
    if (_effect) {
        _effect->update(delta_ms, _outputs, controller);
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
    // Negative conditions work as AND logic: if ANY are true, the rule fails.
    for (uint8_t id : negative_conditions) {
        if (controller.getConditionVariableState(id)) return false;
    }

    // Positive conditions work as OR logic: if ANY are true, the rule passes.
    // If the list is empty, it's considered true.
    if (positive_conditions.empty()) {
        return true;
    }
    for (uint8_t id : positive_conditions) {
        if (controller.getConditionVariableState(id)) return true;
    }

    // If we reach here, no positive conditions were met.
    return false;
}

// --- AuxController ---

AuxController::AuxController() {}

AuxController::~AuxController() {
    reset();
}

void AuxController::addPhysicalOutput(uint8_t pin, OutputType type, uint16_t pwm_frequency, BrightnessCurve curve) {
    _outputs.emplace_back(pin, type, pwm_frequency, curve);
    _outputs.back().attach();
}

void AuxController::update(uint32_t delta_ms) {
    if (_state_changed) {
        evaluateMapping();
        _state_changed = false;
    }
    for (auto& func : _logical_functions) {
        func->update(delta_ms, *this);
    }
}

void AuxController::loadFromCVs(ICVAccess& cvAccess) {
    reset();
    auto mapping_method = static_cast<FunctionMappingMethod>(cvAccess.readCV(CV_FUNCTION_MAPPING_METHOD));
    switch (mapping_method) {
        case FunctionMappingMethod::RCN_225:
            parseRcn225(cvAccess);
            break;
        case FunctionMappingMethod::RCN_227_PER_FUNCTION:
            parseRcn227PerFunction(cvAccess);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V1:
            parseRcn227PerOutputV1(cvAccess);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V2:
            parseRcn227PerOutputV2(cvAccess);
            break;
        case FunctionMappingMethod::PROPRIETARY:
        default:
            return;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V3:
            parseRcn227PerOutputV3(cvAccess);
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

void AuxController::parseRcn225(ICVAccess& cvAccess) {
    const int num_mapping_cvs = CV_OUTPUT_LOCATION_CONFIG_END - CV_OUTPUT_LOCATION_CONFIG_START + 1;
    for (int i = 0; i < num_mapping_cvs; ++i) {
        uint16_t cv_addr = CV_OUTPUT_LOCATION_CONFIG_START + i;
        uint8_t mapping_mask = cvAccess.readCV(cv_addr);
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

/**
 * @brief Parses the RCN-227 "System per Output V3" mapping from CVs.
 * @param cvAccess A reference to the CV access object.
 * @details This is the most advanced RCN-227 mapping system. It uses 8 CVs per
 *          physical output to define a complex set of activating and blocking
 *          conditions based on function keys, direction, and binary states.
 *          This implementation creates one LogicalFunction per configured output
 *          and translates the CVs into a set of ConditionVariables and MappingRules
 *          that correctly implement the OR logic for activation and AND logic for blocking.
 */
void AuxController::parseRcn227PerOutputV3(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 43);

    const int num_outputs = 32;
    uint8_t condition_id_counter = 1; // Start with a non-zero ID

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        PhysicalOutput* po = getOutputById(output_num + 1);
        if (!po) continue;

        uint16_t base_cv = 257 + (output_num * 8);
        std::vector<uint8_t> activating_condition_ids;
        std::vector<uint8_t> blocking_condition_ids;
        bool has_any_rule = false;

        // --- Create all ConditionVariables for this output ---
        // First 4 CVs: F0-F63 with direction
        for (int i = 0; i < 4; ++i) {
            uint8_t cv_value = cvAccess.readCV(base_cv + i);
            if (cv_value == 255) continue;
            has_any_rule = true;

            uint8_t func_num = cv_value & 0x3F;
            uint8_t dir_bits = (cv_value >> 6) & 0x03; // 00=any, 01=fwd, 10=rev, 11=blocking
            bool is_blocking = (dir_bits == 0x03);

            ConditionVariable cv;
            cv.id = condition_id_counter++;
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, func_num});
            if (dir_bits == 0x01) cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            else if (dir_bits == 0x02) cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            addConditionVariable(cv);

            if (is_blocking) {
                blocking_condition_ids.push_back(cv.id);
            } else {
                activating_condition_ids.push_back(cv.id);
            }
        }

        // Next 4 CVs (as two 16-bit pairs): F0-F68 or Binary States
        for (int i = 0; i < 2; ++i) {
            uint8_t cv_high = cvAccess.readCV(base_cv + 4 + (i * 2));
            uint8_t cv_low = cvAccess.readCV(base_cv + 5 + (i * 2));
            if (cv_high == 255 && cv_low == 255) continue;
            has_any_rule = true;

            bool is_blocking = (cv_high & 0x80) != 0;
            uint16_t value = ((cv_high & 0x7F) << 8) | cv_low;

            ConditionVariable cv;
            cv.id = condition_id_counter++;
            if (value <= 68) {
                cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)value});
            } else {
                // Per RCN-227, Binary State 1 is addressed by value 69
                cv.conditions.push_back({TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint8_t)(value - 69)});
            }
            addConditionVariable(cv);

            if (is_blocking) {
                blocking_condition_ids.push_back(cv.id);
            } else {
                activating_condition_ids.push_back(cv.id);
            }
        }

        // --- Create a single LogicalFunction and MappingRules for this output ---
        if (has_any_rule) {
            // Create one LogicalFunction per physical output that is configured
            LogicalFunction* lf = new LogicalFunction(new EffectSteady(255));
            lf->addOutput(po);
            addLogicalFunction(lf);
            uint8_t lf_idx = _logical_functions.size() - 1;

            // This rule handles activation. ANY of the activating conditions can trigger it.
            // All blocking conditions must be false.
            if (!activating_condition_ids.empty()) {
                 MappingRule activation_rule;
                 activation_rule.target_logical_function_id = lf_idx;
                 activation_rule.positive_conditions = activating_condition_ids; // This implies OR logic internally
                 activation_rule.negative_conditions = blocking_condition_ids;
                 activation_rule.action = MappingAction::ACTIVATE;
                 addMappingRule(activation_rule);
            }

            // This rule handles deactivation. If NONE of the activating conditions are met, it deactivates.
            // This is crucial for the logic to work correctly when functions are turned off.
            MappingRule deactivation_rule;
            deactivation_rule.target_logical_function_id = lf_idx;
            deactivation_rule.negative_conditions = activating_condition_ids; // Deactivate if ALL are false
            deactivation_rule.action = MappingAction::DEACTIVATE;
            addMappingRule(deactivation_rule);
        }
    }
}

void AuxController::parseRcn227PerFunction(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 40);

    const int num_functions = 32;

    for (int func_num = 0; func_num < num_functions; ++func_num) {
        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (func_num * 2 + dir) * 4;
            uint32_t output_mask = (uint32_t)cvAccess.readCV(base_cv + 2) << 16 | (uint32_t)cvAccess.readCV(base_cv + 1) << 8 | cvAccess.readCV(base_cv);
            uint8_t blocking_func_num = cvAccess.readCV(base_cv + 3);

            if (output_mask == 0) continue;

            ConditionVariable cv;
            cv.id = (func_num * 2) + dir + 1;
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
            addConditionVariable(cv);

            uint8_t blocking_cv_id = 0;
            if (blocking_func_num != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 100 + blocking_func_num;
                blocking_cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func_num});
                addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int output_bit = 0; output_bit < 24; ++output_bit) {
                if ((output_mask >> output_bit) & 1) {
                    uint8_t physical_output_id = output_bit + 1;
                    LogicalFunction* lf = new LogicalFunction(new EffectSteady(255));
                    lf->addOutput(getOutputById(physical_output_id));
                    addLogicalFunction(lf);
                    uint8_t lf_idx = _logical_functions.size() - 1;

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) rule.negative_conditions.push_back(blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV1(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 41);

    const int num_outputs = 24;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr; // Lazily created

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint32_t func_mask = (uint32_t)cvAccess.readCV(base_cv + 3) << 24 | (uint32_t)cvAccess.readCV(base_cv + 2) << 16 | (uint32_t)cvAccess.readCV(base_cv + 1) << 8 | cvAccess.readCV(base_cv);

            if (func_mask == 0) continue;

            if (lf == nullptr) {
                lf = new LogicalFunction(new EffectSteady(255));
                lf->addOutput(getOutputById(output_num + 1));
                addLogicalFunction(lf);
            }
            uint8_t lf_idx = _logical_functions.size() - 1;

            for (int func_num = 0; func_num < 32; ++func_num) {
                if ((func_mask >> func_num) & 1) {
                    ConditionVariable cv;
                    cv.id = 200 + (output_num * 64) + (dir * 32) + func_num; // Unique ID
                    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
                    cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV2(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 42);

    const int num_outputs = 32;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr;

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint8_t funcs[] = {
                cvAccess.readCV(base_cv),
                cvAccess.readCV(base_cv + 1),
                cvAccess.readCV(base_cv + 2)
            };
            uint8_t blocking_func = cvAccess.readCV(base_cv + 3);

            uint8_t blocking_cv_id = 0;
            if (blocking_func != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 400 + blocking_func; // Unique ID
                blocking_cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func});
                addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int i = 0; i < 3; ++i) {
                if (funcs[i] != 255) {
                    if (lf == nullptr) {
                        lf = new LogicalFunction(new EffectSteady(255));
                        lf->addOutput(getOutputById(output_num + 1));
                        addLogicalFunction(lf);
                    }
                    uint8_t lf_idx = _logical_functions.size() - 1;

                    ConditionVariable cv;
                    cv.id = 500 + (output_num * 8) + (dir * 4) + i; // Unique ID
                    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, funcs[i]});
                    cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) rule.negative_conditions.push_back(blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}
}
