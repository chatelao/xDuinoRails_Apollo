/**
 * @file config.h
 * @brief Zentrale Konfigurationsdatei für den Lokdecoder.
 *
 * In dieser Datei werden alle wichtigen Einstellungen für den Betrieb des Decoders
 * vorgenommen. Dazu gehören die Auswahl des Digitalprotokolls (DCC oder MM),
 * die Zuweisung der Hardware-Pins für die Motorsteuerung und die Konfiguration
 * der Fahreigenschaften des Motors.
 *
 * @note Für PlatformIO-Benutzer werden einige dieser Einstellungen, insbesondere
 * die Protokollauswahl, durch die Umgebung in der `platformio.ini`-Datei
 * überschrieben. Arduino-IDE-Benutzer müssen die Konfiguration hier manuell
 * vornehmen.
 */

#ifndef CONFIG_H
#define CONFIG_H

// =====================================================================================
// Protokoll-Konfiguration
// =====================================================================================
// Wählen Sie hier das zu verwendende Protokoll aus, indem Sie eine der folgenden
// Zeilen auskommentieren oder einkommentieren. Es darf immer nur ein Protokoll
// aktiv sein.
//
// Für PlatformIO-Benutzer: Diese Einstellung wird durch die Umgebung in
// `platformio.ini` automatisch überschrieben.
//
// Für Arduino-IDE-Benutzer: Stellen Sie sicher, dass die benötigten Bibliotheken
// (z.B. NmraDcc, MaerklinMotorola) manuell über den Bibliotheksverwalter
// installiert sind.

/**
 * @brief Definiert das zu verwendende Digitalprotokoll.
 *
 * - `PROTOCOL_MM`: Aktiviert das Märklin-Motorola-Protokoll.
 * - `PROTOCOL_DCC`: Aktiviert das DCC-Protokoll.
 *
 * @warning Es darf immer nur ein Protokoll zur gleichen Zeit aktiviert sein.
 */
#define PROTOCOL_MM
// #define PROTOCOL_DCC


// =====================================================================================
// Hardware-Pin-Konfiguration
// =====================================================================================
// Definieren Sie hier die Pins, die für die Ansteuerung der Motor-H-Brücke
// verwendet werden.

/**
 * @brief Pin für das PWM-Signal 'A' der Motor-H-Brücke.
 * Dieser Pin steuert die Geschwindigkeit des Motors.
 */
#define MOTOR_PIN_A 0

/**
 * @brief Pin für das PWM-Signal 'B' der Motor-H-Brücke.
 * Dieser Pin wird zusammen mit MOTOR_PIN_A zur Steuerung der Drehrichtung verwendet.
 */
#define MOTOR_PIN_B 1

/**
 * @brief Analoger Eingangspin zur Messung der Gegen-EMK (BEMF) an Motoranschluss A.
 * Wird für die erweiterte Lastregelung des Motors verwendet.
 */
#define MOTOR_BEMF_A_PIN A3

/**
 * @brief Analoger Eingangspin zur Messung der Gegen-EMK (BEMF) an Motoranschluss B.
 * Wird für die erweiterte Lastregelung des Motors verwendet.
 */
#define MOTOR_BEMF_B_PIN A2

// =====================================================================================
// Funktions- & Ausgangs-Konfiguration (Function Mapping)
// =====================================================================================
// In diesem Abschnitt wird die gesamte Konfiguration der Licht- und Sonderfunktionen
// vorgenommen. Das System ist in drei Stufen aufgebaut:
//
// 1. Physische Ausgänge (Physical Outputs):
//    Definieren Sie hier alle Hardware-Pins, die als Ausgänge verwendet werden sollen.
//
// 2. Logische Funktionen (Logical Functions):
//    Eine logische Funktion ist eine Eigenschaft der Lok (z.B. "Stirnlicht vorne").
//    Jeder logischen Funktion wird ein Licht-Effekt (z.B. "Dauerlicht") und
//    mindestens ein physischer Ausgang zugewiesen.
//
// 3. Mapping:
//    Hier wird festgelegt, welche Taste auf der Zentrale (z.B. F0) welche
//    logische Funktion steuert.
//
// HINWEIS FÜR PHASE 1: Das Mapping ist noch direkt. F0 steuert die erste
// definierte logische Funktion, F1 die zweite, und so weiter.
// =====================================================================================

// --- 1. Physische Ausgänge definieren ---
// Jeder Ausgang erhält einen eindeutigen Namen und eine Pin-Nummer.
#define PO_HEADLIGHT_FWD 26 // Stirnlicht vorne
#define PO_HEADLIGHT_REV 27 // Stirnlicht hinten
#define PO_CABIN_LIGHT 28   // Führerstandsbeleuchtung (Beispiel)

// --- 2. Logische Funktionen und Effekte definieren ---
// Hier wird die Setup-Logik für den FunctionManager vorbereitet.
// Dies geschieht in der `setup()`-Funktion in `main.cpp`.
//
// Beispiel-Konfiguration:
// - F0: Richtungsabhängiges Stirnlicht (noch nicht implementiert, für Phase 1 nur einfaches An/Aus)
// - F1: Führerstandsbeleuchtung an/aus
//
// WICHTIG: Die Reihenfolge der Aufrufe von `functionManager.addLogicalFunction`
// in `main.cpp` bestimmt das Mapping:
// - Der erste Aufruf wird von F0 gesteuert.
// - Der zweite Aufruf wird von F1 gesteuert.
// - usw.

// Helligkeits-Definitionen
#define BRIGHTNESS_FULL 255
#define BRIGHTNESS_DIMMED 80


// =====================================================================================
// Motor-Konfiguration für xDuinoRails_MotorControl
// =====================================================================================
// Die primären Motoreinstellungen (Beschleunigung, Bremszeit, Anfahrspannung)
// werden nun über DCC-CVs (CV 2, 3, 4) konfiguriert. Die hier verbleibenden
// Werte sind für speziellere Anpassungen.

/**
 * @brief Dauer des "Startup Kick" in Millisekunden.
 * Definiert, wie lange der unter CV 2 (`MOTOR_STARTUP_KICK_PWM`) definierte Impuls
 * angelegt wird. Dieser Wert ist nicht per CV änderbar.
 */
#define MOTOR_STARTUP_KICK_DURATION 10


#endif // CONFIG_H
