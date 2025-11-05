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


#endif // CONFIG_H
