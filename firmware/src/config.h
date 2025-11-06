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

#define PROTOCOL_MM
// #define PROTOCOL_DCC


// =====================================================================================
// Hardware-Pin-Konfiguration
// =====================================================================================
// Definieren Sie hier die Pins, die für die Ansteuerung der Motor-H-Brücke
// verwendet werden.

// Pin für das PWM-Signal (Geschwindigkeit)
#define MOTOR_PIN_A 0

// Pin für die Richtungsumschaltung
#define MOTOR_PIN_B 1

// Pins für die BEMF-Messung (Analog-Eingänge)
#define MOTOR_BEMF_A_PIN A3
#define MOTOR_BEMF_B_PIN A2

// =====================================================================================
// Konfiguration der Funktionsausgänge (Licht & AUX)
// =====================================================================================
// Dieses neue System ersetzt die alte, statische Licht-Konfiguration.
// Es ermöglicht eine flexible Zuweisung von Funktionen (z.B. Licht, Rauch)
// zu Ausgängen und deren Steuerung über F-Tasten.

// Schritt 1: Definieren Sie die physischen Ausgangspins, die Sie verwenden möchten.
#define FUNC_PHYSICAL_PIN_0 26 // z.B. Licht vorne
#define FUNC_PHYSICAL_PIN_1 27 // z.B. Licht hinten
// #define FUNC_PHYSICAL_PIN_2 28 // z.B. Führerstandsbeleuchtung

// Schritt 2: Konfigurieren Sie die "Logischen Funktionen".
// Jede logische Funktion kombiniert einen physischen Pin mit einem Verhalten (Effekt)
// und mappt dies auf eine F-Taste und optional auf die Fahrtrichtung.

// --- Logische Funktion 0 (z.B. Licht Vorne) ---
#define LOGICAL_FUNC_0_TYPE         STEADY // Effekt-Typ (STEADY oder DIMMING)
#define LOGICAL_FUNC_0_OUTPUT_PIN   FUNC_PHYSICAL_PIN_0 // Welcher Pin wird angesteuert?
#define LOGICAL_FUNC_0_MAPPED_KEY   0      // Welche F-Taste steuert die Funktion? (F0)
#define LOGICAL_FUNC_0_DIRECTION    1      // Richtungsabhängigkeit: 1=Vorwärts, -1=Rückwärts, 0=Immer an
#define LOGICAL_FUNC_0_PARAM_1      255    // Parameter 1 (z.B. Helligkeit für STEADY)

// --- Logische Funktion 1 (z.B. Licht Hinten) ---
#define LOGICAL_FUNC_1_TYPE         STEADY
#define LOGICAL_FUNC_1_OUTPUT_PIN   FUNC_PHYSICAL_PIN_1
#define LOGICAL_FUNC_1_MAPPED_KEY   0
#define LOGICAL_FUNC_1_DIRECTION    -1
#define LOGICAL_FUNC_1_PARAM_1      255

// Um eine weitere Funktion hinzuzufügen, kopieren Sie einfach einen Block und
// erhöhen die Ziffer (z.B. LOGICAL_FUNC_2_...).


// =====================================================================================
// Motor-Konfiguration für xDuinoRails_MotorControl
// =====================================================================================
// Hier werden die Parameter für die erweiterte Motorsteuerung definiert. Passen
// Sie diese Werte an, um das Fahrverhalten Ihrer Lokomotive zu optimieren.

// Beschleunigungsrate in "Pulsen pro Sekunde pro Sekunde" (PPS^2).
// Ein höherer Wert bedeutet eine schnellere Beschleunigung.
// Ein Wert von 50 bedeutet, dass die Geschwindigkeit jede Sekunde um 50 PPS zunimmt.
#define MOTOR_ACCELERATION 50

// Bremsrate in "Pulsen pro Sekunde pro Sekunde" (PPS^2).
// Ein höherer Wert bedeutet eine stärkere Bremsung.
// Ein Wert von 100 bedeutet, dass die Geschwindigkeit jede Sekunde um 100 PPS abnimmt.
#define MOTOR_DECELERATION 100

// "Startup Kick" zum Überwinden der anfänglichen Trägheit des Motors.
// MOTOR_STARTUP_KICK_PWM: Der PWM-Wert (0-255), der kurz angelegt wird.
// MOTOR_STARTUP_KICK_DURATION: Die Dauer des Kicks in Millisekunden.
#define MOTOR_STARTUP_KICK_PWM 80
#define MOTOR_STARTUP_KICK_DURATION 10


#endif // CONFIG_H
