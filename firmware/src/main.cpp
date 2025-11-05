#include <Arduino.h>
#include "config.h" // Die neue zentrale Konfigurationsdatei einbinden

// =====================================================================================
// DCC Protocol Implementation
// =====================================================================================
#ifdef PROTOCOL_DCC

#define NMRA_DCC_PROCESS_MULTIFUNCTION
#include <NmraDcc.h>

// Pin-Zuweisungen für DCC
#define DCC_SIGNAL_PIN 7
#define HEADLIGHT_PIN 3 // Beispiel für eine Funktion

// Ein DCC-Objekt erstellen
NmraDcc dcc;

// Callback-Funktion für DCC-Geschwindigkeit und -Richtung
void notifyDccSpeed(uint16_t Addr, DCC_ADDR_TYPE AddrType, uint8_t Speed, DCC_DIRECTION Dir, DCC_SPEED_STEPS SpeedSteps) {
  if (Addr == dcc.getAddr()) {
    // Motorsteuerung basierend auf den Pins aus config.h
    // Annahme: MOTOR_PIN_A ist PWM, MOTOR_PIN_B ist Richtung.
    if (Dir == DCC_DIR_FWD) {
      analogWrite(MOTOR_PIN_A, Speed);
      digitalWrite(MOTOR_PIN_B, LOW);
    } else {
      digitalWrite(MOTOR_PIN_A, LOW);
      analogWrite(MOTOR_PIN_B, Speed);
    }
  }
}

// Callback-Funktion für DCC-Funktionen
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState) {
  if (Addr == dcc.getAddr()) {
    if (FuncGrp == FN_0_4) {
      if (FuncState & FN_BIT_00) {
        digitalWrite(HEADLIGHT_PIN, HIGH);
      } else {
        digitalWrite(HEADLIGHT_PIN, LOW);
      }
    }
  }
}

void setup() {
  // Pin-Modi setzen
  pinMode(MOTOR_PIN_A, OUTPUT);
  pinMode(MOTOR_PIN_B, OUTPUT);
  pinMode(HEADLIGHT_PIN, OUTPUT);

  // DCC-Bibliothek initialisieren
  dcc.pin(DCC_SIGNAL_PIN, false);
  dcc.init(MAN_ID_DIY, 1, FLAGS_MY_ADDRESS_ONLY, 0);

  // Decoder-Adresse setzen
  dcc.setCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS, 3);
}

void loop() {
  // DCC-Pakete verarbeiten
  dcc.process();
}

#endif // PROTOCOL_DCC


// =====================================================================================
// Märklin-Motorola (MM) Protocol Implementation
// =====================================================================================
#ifdef PROTOCOL_MM

#include <MaerklinMotorola.h>

// MM-spezifische Einstellungen
#define MM_ADDRESS 5
#define MM_SIGNAL_PIN 7

// Globales Objekt für den MM-Decoder erstellen
MaerklinMotorola MM(MM_SIGNAL_PIN);

// Zustandsvariable für die aktuelle Fahrtrichtung
static bool s_direction_is_forward = true;

// Interrupt Service Routine (ISR) für die Signalverarbeitung
void mm_isr() {
  MM.PinChange();
}

void setup() {
  // Pin-Modi für den Motor setzen
  pinMode(MOTOR_PIN_A, OUTPUT);
  pinMode(MOTOR_PIN_B, OUTPUT);

  // Interrupt an den Signal-Pin binden
  attachInterrupt(digitalPinToInterrupt(MM_SIGNAL_PIN), mm_isr, CHANGE);
}

void loop() {
  // Empfangene Daten parsen
  MM.Parse();

  // Überprüfen, ob ein validiertes Datenpaket vorhanden ist
  MaerklinMotorolaData* data = MM.GetData();

  if (data && !data->IsMagnet && data->Address == MM_ADDRESS) {
    // Befehl verarbeiten

    if (data->ChangeDir) {
      // Fahrtrichtung umschalten und Motor anhalten
      s_direction_is_forward = !s_direction_is_forward;
      analogWrite(MOTOR_PIN_A, 0);
      digitalWrite(MOTOR_PIN_B, 0);
    } else if (data->Stop) {
      // Motor anhalten
      analogWrite(MOTOR_PIN_A, 0);
      digitalWrite(MOTOR_PIN_B, 0);
    } else {
      // Geschwindigkeitsbefehl
      // MM-Geschwindigkeit (0-14) auf den PWM-Bereich (0-255) abbilden
      uint8_t pwm_speed = map(data->Speed, 0, 14, 0, 255);

      // Motor basierend auf der gespeicherten Richtung ansteuern
      if (s_direction_is_forward) {
        analogWrite(MOTOR_PIN_A, pwm_speed);
        digitalWrite(MOTOR_PIN_B, LOW);
      } else {
        digitalWrite(MOTOR_PIN_A, LOW);
        analogWrite(MOTOR_PIN_B, pwm_speed);
      }
    }
  }
}

#endif // PROTOCOL_MM
