# Checkliste für den Märklin-Motorola (MM) Decoder

Dieses Dokument beschreibt die notwendigen Schritte, um einen voll funktionsfähigen Märklin-Motorola-Decoder auf Basis des Seeed Studio XIAO RP2040 zu entwickeln.

## Phase 1: Minimal Viable Product (MVP) - MM-Funktionalität

Ziel dieser Phase ist es, einen Decoder zu erstellen, der eine Lok mit dem Märklin-Motorola-Protokoll steuern kann (nur Fahren).

- [x] **1. Projekt-Setup und Bibliotheks-Integration**
    - [x] Die `LaserLight/MaerklinMotorola` Bibliothek als Git-Submodul oder über den PlatformIO Library Manager in das Projekt integrieren.
    - [x] Eine Umschaltmöglichkeit zwischen DCC- und MM-Protokoll mittels Compiler-Flags implementieren.
        - [x] **Vorschlag:** In `platformio.ini` zwei Umgebungen definieren: `[env:xiao_dcc]` und `[env:xiao_mm]`.
        - [x] **Vorschlag:** Build-Flags `-DPROTOCOL_DCC` und `-DPROTOCOL_MM` in den jeweiligen Umgebungen setzen.
    - [x] Sicherstellen, dass der Code für beide Protokolle separat kompiliert werden kann, indem die protokollspezifische Logik mit `#ifdef PROTOCOL_DCC` / `#endif` und `#ifdef PROTOCOL_MM` / `#endif` umschlossen wird.
    - [x] Die `NmraDcc` Bibliothek nur dann einbinden und initialisieren, wenn `PROTOCOL_DCC` definiert ist.
    - [x] Die `MaerklinMotorola` Bibliothek nur dann einbinden und initialisieren, wenn `PROTOCOL_MM` definiert ist.

- [x] **2. Implementierung der Motorsteuerung (Open-Loop PWM)**
    - [x] Die Hardware-Pins für die H-Brücken-Ansteuerung am XIAO RP2040 definieren.
    - [x] Eine einfache PWM-Funktionalität implementieren, um die Geschwindigkeit des Motors zu regeln.
    - [x] Eine Funktion erstellen, die die Fahrtrichtung durch Umpolen der H-Brücke steuert.
    - [x] Die Geschwindigkeits- und Richtungsbefehle aus der `MaerklinMotorola` Bibliothek auslesen und an die PWM- und Richtungs-Logik weitergeben.

- [x] **3. GitHub Actions Workflow anpassen**
    - [x] Die `build.yml` Workflow-Datei erweitern.
    - [x] Zwei separate Build-Jobs erstellen: einen für die DCC-Version (`pio run -e xiao_dcc`) und einen für die MM-Version (`pio run -e xiao_mm`).
    - [x] Die kompilierten Firmware-Dateien (`firmware.bin` oder `firmware.uf2`) beider Jobs als separate "Release Assets" oder Build-Artefakte speichern (z.B. `firmware_dcc.bin` und `firmware_mm.bin`).

## Phase 2: Zukünftige Erweiterungen

- [x] **1. Fortgeschrittene Motorsteuerung**
    - [x] Die `chatelao/xDuinoRails_MotorControl` Bibliothek in das Projekt integrieren.
    - [x] Die einfache PWM-Steuerung durch die fortgeschrittene Regelung (z.B. PID-Regelung für Lastausgleich) aus der neuen Bibliothek ersetzen.

- [x] **2. Funktionsausgänge**
    - [x] Ansteuerung für mindestens einen Funktionsausgang (z.B. für Licht) implementieren.
    - [x] Die Funktionsbefehle (F0, F1, etc.) aus der `MaerklinMotorola` Bibliothek auswerten.

- [ ] **3. Multiprotokoll-Fähigkeit (Gleichzeitiger Betrieb)**
    - [ ] Den Decoder so erweitern, dass er zur Laufzeit automatisch zwischen DCC- und MM-Signalen unterscheiden kann.
    - [ ] Eine gemeinsame Schnittstelle für die Motor- und Funktionssteuerung schaffen, die von beiden Protokoll-Implementierungen genutzt werden kann.

- [ ] **4. Konfigurierbarkeit**
    - [ ] Eine Möglichkeit schaffen, die MM-Adresse des Decoders zu programmieren (z.B. über einen "Programmiermodus").
    - [ ] Konfigurierbare Parameter wie Anfahr- und Bremsverzögerung (ABV) hinzufügen.

## Phase 3: Implementierung von Licht- und Aux-Funktionen

- [ ] **1. Kern-Infrastruktur**
    - [ ] Implementierung der Kern-Klassen für physische Ausgänge und logische Funktionen.
    - [ ] Implementierung der `LightEffect`-Basisklasse und der einfachsten Effekte: `Konstant` und `Dimmen`.
    - [ ] Implementierung eines einfachen, direkten Mapping-Systems, um F-Tasten das Ein- und Ausschalten von logischen Funktionen zu ermöglichen.
- [ ] **2. Erweiterte Effekte & Mapping**
    - [ ] Implementierung der vollständigen, mehrstufigen Tabellenlogik für das Funktions-Mapping.
    - [ ] Implementierung aller verbleibenden erweiterten Lichteffekte (Flimmern, Blitz, Marslicht, etc.).
    - [ ] Entwicklung einer umfassenden CV-Tabelle zur Konfiguration aller neuen Funktionen.
- [ ] **3. Hilfsfunktionen**
    - [ ] Implementierung des `SERVO_CONTROL`-Effekts.
    - [ ] Implementierung des `SMOKE_GENERATOR`-Effekts, einschließlich geschwindigkeitssynchronisierter Lüftersteuerung.
    - [ ] Hinzufügen von CVs zur Konfiguration dieser neuen Hilfsfunktionen.
- [ ] **4. Werkzeuge & Verfeinerung**
    - [ ] Entwicklung eines PC-basierten Konfigurationstools zur Vereinfachung der CV-Programmierung.
    - [ ] Erstellung einer umfassenden Benutzerdokumentation für alle Funktionen.
    - [ ] Verfeinerung und Optimierung des Codes basierend auf Community-Feedback.

## Phase 4: Implementierung des Sound-Systems

- [ ] **1. Kern-Audio-Infrastruktur**
    - [ ] Implementierung der grundlegenden Hardware-Treiber (I2S, DMA) und eines WAV-Readers.
    - [ ] Erstellung eines einfachen Prototyps, der einen einzelnen Sound per Funktionstaste abspielen kann.
- [ ] **2. Polyphonie & Prime Mover**
    - [ ] Implementierung eines Software-Mixers für die gleichzeitige Wiedergabe mehrerer Sounds (Polyphonie).
    - [ ] Implementierung eines dynamischen `PRIME_MOVER`-Sounds für Diesel-Lokomotiven.
- [ ] **3. Erweitertes Mapping & Spezial-Lokomotiven**
    - [ ] Implementierung der vollständigen Tabellenlogik für das flexible Sound-Mapping.
    - [ ] Implementierung der `PRIME_MOVER`-Logik für Dampf- (mit BEMF-Synchronisation) und Elektro-Lokomotiven.
- [ ] **4. Werkzeuge & "Sound-Font"-Verwaltung**
    - [ ] Entwicklung eines PC-basierten Tools zur Erstellung und Verwaltung von "Sound-Fonts" (Sound-Pakete).
    - [ ] Erstellung einer umfassenden Benutzerdokumentation.
