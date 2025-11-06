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
// Motor-Konfiguration für xDuinoRails_MotorControl
// =====================================================================================
// Hier werden die Parameter für die erweiterte Motorsteuerung definiert. Passen
// Sie diese Werte an, um das Fahrverhalten Ihrer Lokomotive zu optimieren.

/**
 * @brief Beschleunigungsrate in "Pulsen pro Sekunde pro Sekunde" (PPS^2).
 * Definiert, wie schnell die Lokomotive ihre Zielgeschwindigkeit erreicht.
 * Ein höherer Wert bedeutet eine schnellere Beschleunigung.
 * Beispiel: Ein Wert von 50 bedeutet, dass die Geschwindigkeit jede Sekunde um 50 PPS zunimmt.
 */
#define MOTOR_ACCELERATION 50

/**
 * @brief Bremsrate in "Pulsen pro Sekunde pro Sekunde" (PPS^2).
 * Definiert, wie schnell die Lokomotive abbremst.
 * Ein höherer Wert bedeutet eine stärkere (schnellere) Bremsung.
 * Beispiel: Ein Wert von 100 bedeutet, dass die Geschwindigkeit jede Sekunde um 100 PPS abnimmt.
 */
#define MOTOR_DECELERATION 100

/**
 * @brief PWM-Wert (0-255) für den "Startup Kick".
 * Ein kurzer, starker Impuls, um die anfängliche Trägheit des Motors zu überwinden
 * und ein sanftes Anfahren zu ermöglichen, besonders bei niedrigen Geschwindigkeiten.
 */
#define MOTOR_STARTUP_KICK_PWM 80

/**
 * @brief Dauer des "Startup Kick" in Millisekunden.
 * Definiert, wie lange der unter `MOTOR_STARTUP_KICK_PWM` definierte Impuls
 * angelegt wird.
 */
#define MOTOR_STARTUP_KICK_DURATION 10


#endif // CONFIG_H
