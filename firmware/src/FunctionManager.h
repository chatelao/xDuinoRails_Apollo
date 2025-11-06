#ifndef FUNCTION_MANAGER_H
#define FUNCTION_MANAGER_H

#include <Arduino.h>

// --- Core Effect Classes ---

/**
 * @file FunctionManager.h
 * @brief Definiert die Klassen für das Management von Licht- und Sonderfunktionen.
 */

/**
 * @class LightEffect
 * @brief Abstrakte Basisklasse für alle Licht- und Funktionseffekte.
 */
class LightEffect {
public:
    virtual ~LightEffect() {}
    virtual void update(uint32_t delta_ms) = 0;
    virtual uint8_t getPwmValue() = 0;
    virtual void setActive(bool active) { this->active = active; }
    bool isActive() const { return active; }

protected:
    bool active = false;
};

/**
 * @class EffectSteady
 * @brief Implementiert einen "Dauerlicht"-Effekt mit konstanter Helligkeit.
 */
class EffectSteady : public LightEffect {
public:
    EffectSteady(uint8_t brightness);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
private:
    uint8_t brightness;
};

/**
 * @class EffectDimming
 * @brief Implementiert einen Effekt zum Umschalten zwischen zwei Helligkeiten.
 */
class EffectDimming : public LightEffect {
public:
    EffectDimming(uint8_t brightness_full, uint8_t brightness_dimmed);
    void update(uint32_t delta_ms) override;
    uint8_t getPwmValue() override;
    void setDimmed(bool dimmed);
private:
    uint8_t brightness_full;
    uint8_t brightness_dimmed;
    bool is_dimmed = false;
};


// --- Core Infrastructure Classes ---

/**
 * @class PhysicalOutput
 * @brief Repräsentiert einen physischen Ausgangspin des Mikrocontrollers.
 */
class PhysicalOutput {
public:
    PhysicalOutput(uint8_t pin);
    void begin();
    void write(uint8_t value);
private:
    uint8_t pin;
    uint8_t last_value = 255; // Initialwert, um ein erstes Schreiben zu erzwingen
};

/**
 * @class LogicalFunction
 * @brief Verbindet einen Effekt mit einem physischen Ausgang.
 */
class LogicalFunction {
public:
    LogicalFunction(PhysicalOutput* output, LightEffect* effect);
    ~LogicalFunction();
    void update();
    void setActive(bool active);
private:
    PhysicalOutput* output;
    LightEffect* effect;
    uint32_t last_update_ms = 0;
};


// --- Management Class ---

#define MAX_LOGICAL_FUNCTIONS 16 // Unterstützt F0-F15

/**
 * @class FunctionManager
 * @brief Verwaltet alle logischen Funktionen und deren Lebenszyklen.
 */
class FunctionManager {
public:
    FunctionManager();
    ~FunctionManager();

    /**
     * @brief Registriert eine neue logische Funktion und verknüpft sie mit einer F-Taste.
     * @param function_key Die F-Taste (0-28), die diese Funktion auslöst.
     * @param output Zeiger auf das PhysicalOutput-Objekt.
     * @param effect Zeiger auf das LightEffect-Objekt.
     * @param direction_dependency Gibt an, ob diese Funktion richtungsabhängig ist.
     *        0 = immer aktiv (wenn F-Taste an), 1 = nur bei Vorwärtsfahrt, -1 = nur bei Rückwärtsfahrt.
     */
    void registerFunction(uint8_t function_key, PhysicalOutput* output, LightEffect* effect, int8_t direction_dependency = 0);

    /**
     * @brief Initialisiert alle zugehörigen Ausgänge.
     */
    void begin();

    /**
     * @brief Aktualisiert den Zustand aller registrierten logischen Funktionen.
     *        Sollte in der Hauptschleife (loop) aufgerufen werden.
     */
    void update();

    /**
     * @brief Setzt den Zustand einer F-Taste (gedrückt/nicht gedrückt).
     * @param function_key Die Nummer der F-Taste (0-28).
     * @param state Der Zustand (true für an, false für aus).
     */
    void setFunctionKeyState(uint8_t function_key, bool state);

    /**
     * @brief Setzt die aktuelle Fahrtrichtung des Decoders.
     * @param forward True für Vorwärtsfahrt, false für Rückwärtsfahrt.
     */
    void setDirection(bool forward);

private:
    struct FunctionMapping {
        LogicalFunction* logical_function;
        int8_t direction_dependency;
        bool key_state;
    };

    FunctionMapping* function_map[MAX_LOGICAL_FUNCTIONS];
    bool current_direction_is_forward = true;
};

#endif // FUNCTION_MANAGER_H
