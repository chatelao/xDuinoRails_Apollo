#include "FunctionManager.h"

// --- EffectSteady ---
EffectSteady::EffectSteady(uint8_t brightness) : brightness(brightness) {}

void EffectSteady::update(uint32_t delta_ms) {
    // Nichts zu tun, Helligkeit ist konstant
}

uint8_t EffectSteady::getPwmValue() {
    return active ? brightness : 0;
}


// --- EffectDimming ---
EffectDimming::EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed)
    : brightness_full(brightness_full), brightness_dimmed(brightness_dimmed) {}

void EffectDimming::update(uint32_t delta_ms) {
    // Nichts zu tun, Helligkeit ist zustandsabhängig
}

uint8_t EffectDimming::getPwmValue() {
    if (!active) return 0;
    return is_dimmed ? brightness_dimmed : brightness_full;
}

void EffectDimming::setDimmed(bool dimmed) {
    is_dimmed = dimmed;
}


// --- PhysicalOutput ---
PhysicalOutput::PhysicalOutput(uint8_t pin) : pin(pin) {}

void PhysicalOutput::begin() {
    pinMode(pin, OUTPUT);
    analogWrite(pin, 0);
    last_value = 0;
}

void PhysicalOutput::write(uint8_t value) {
    if (value != last_value) {
        analogWrite(pin, value);
        last_value = value;
    }
}


// --- LogicalFunction ---
LogicalFunction::LogicalFunction(PhysicalOutput* output, LightEffect* effect)
    : output(output), effect(effect) {}

LogicalFunction::~LogicalFunction() {
    delete effect;
}

void LogicalFunction::update() {
    uint32_t now = millis();
    uint32_t delta = now - last_update_ms;
    last_update_ms = now;

    effect->update(delta);
    output->write(effect->getPwmValue());
}

void LogicalFunction::setActive(bool active) {
    effect->setActive(active);
}


// --- FunctionManager ---
FunctionManager::FunctionManager() {
    // The vector is empty by default, no need to initialize
}

FunctionManager::~FunctionManager() {
    for (int i = 0; i < MAX_LOGICAL_FUNCTIONS; ++i) {
        for (auto mapping : function_map[i]) {
            delete mapping->logical_function;
            delete mapping;
        }
    }
}

void FunctionManager::registerFunction(uint8_t function_key, PhysicalOutput* output, LightEffect* effect, int8_t direction_dependency) {
    if (function_key >= MAX_LOGICAL_FUNCTIONS) return;

    LogicalFunction* lf = new LogicalFunction(output, effect);
    FunctionMapping* mapping = new FunctionMapping{lf, direction_dependency, false};
    function_map[function_key].push_back(mapping);
}

void FunctionManager::begin() {
    // Diese Funktion könnte verwendet werden, um alle PhysicalOutputs zu initialisieren,
    // aber die `begin`-Methode von PhysicalOutput wird derzeit direkt aufgerufen.
    // Vorerst leer lassen.
}

void FunctionManager::update() {
    for (int i = 0; i < MAX_LOGICAL_FUNCTIONS; ++i) {
        for (auto mapping : function_map[i]) {
            bool is_active = mapping->key_state;

            // Richtungsabhängigkeit prüfen
            if (mapping->direction_dependency != 0) {
                if (mapping->direction_dependency == 1 && !current_direction_is_forward) {
                    is_active = false;
                } else if (mapping->direction_dependency == -1 && current_direction_is_forward) {
                    is_active = false;
                }
            }

            mapping->logical_function->setActive(is_active);
            mapping->logical_function->update();
        }
    }
}

void FunctionManager::setFunctionKeyState(uint8_t function_key, bool state) {
    if (function_key < MAX_LOGICAL_FUNCTIONS) {
        for (auto mapping : function_map[function_key]) {
            mapping->key_state = state;
        }
    }
}

void FunctionManager::setDirection(bool forward) {
    current_direction_is_forward = forward;
}
