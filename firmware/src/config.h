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
// Licht-Konfiguration
// =====================================================================================
// Definieren Sie hier die Pins für die richtungsabhängige Beleuchtung.
#define LIGHT_PIN_FWD 26 // Licht vorne
#define LIGHT_PIN_REV 27 // Licht hinten
#define LIGHT_BRIGHTNESS 255 // Helligkeit (0-255)


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
