# Projekt-Roadmap

Dieses Dokument beschreibt die notwendigen Schritte, um einen voll funktionsfähigen Multiprotokoll-Lokdecoder auf Basis des Seeed Studio XIAO RP2040 zu entwickeln.

## Phase 1: Minimal Viable Product (MVP)

- [x] **1. Projekt-Setup und Bibliotheks-Integration**
    - [x] Die `LaserLight/MaerklinMotorola` Bibliothek als Git-Submodul oder über den PlatformIO Library Manager in das Projekt integrieren.
    - [x] Eine Umschaltmöglichkeit zwischen DCC- und MM-Protokoll mittels Compiler-Flags implementieren.
    - [x] Sicherstellen, dass der Code für beide Protokolle separat kompiliert werden kann.
- [x] **2. Implementierung der Motorsteuerung (Open-Loop PWM)**
    - [x] Die Hardware-Pins für die H-Brücken-Ansteuerung am XIAO RP2040 definieren.
    - [x] Eine einfache PWM-Funktionalität implementieren, um die Geschwindigkeit des Motors zu regeln.
    - [x] Die Geschwindigkeits- und Richtungsbefehle aus der `MaerklinMotorola` Bibliothek auslesen und an die PWM- und Richtungs-Logik weitergeben.
- [x] **3. GitHub Actions Workflow anpassen**
    - [x] Zwei separate Build-Jobs erstellen: einen für die DCC-Version (`pio run -e xiao_dcc`) und einen für die MM-Version (`pio run -e xiao_mm`).
    - [x] Die kompilierten Firmware-Dateien beider Jobs als separate "Release Assets" oder Build-Artefakte speichern.

## Phase 2: Funktionserweiterungen

- [x] **1. Fortgeschrittene Motorsteuerung**
    - [x] Die `chatelao/xDuinoRails_MotorControl` Bibliothek in das Projekt integrieren.
    - [x] Die einfache PWM-Steuerung durch die fortgeschrittene Regelung (z.B. PID-Regelung für Lastausgleich) ersetzen.
- [x] **2. Funktionsausgänge**
    - [x] Ansteuerung für mindestens einen Funktionsausgang (z.B. für Licht) implementieren.
    - [x] Die Funktionsbefehle (F0, F1, etc.) aus der `MaerklinMotorola` Bibliothek auswerten.
- [ ] **3. Multiprotokoll-Fähigkeit (Gleichzeitiger Betrieb)**
    - [ ] Den Decoder so erweitern, dass er zur Laufzeit automatisch zwischen DCC- und MM-Signalen unterscheiden kann.
    - [ ] Eine gemeinsame Schnittstelle für die Motor- und Funktionssteuerung schaffen, die von beiden Protokoll-Implementierungen genutzt werden kann.
- [ ] **4. Konfigurierbarkeit**
    - [ ] Eine Möglichkeit schaffen, die MM-Adresse des Decoders zu programmieren.
    - [ ] Konfigurierbare Parameter wie Anfahr- und Bremsverzögerung (ABV) hinzufügen.

## Phase 3: Implementierung von Licht- und Aux-Funktionen

- [x] **1. Kern-Infrastruktur**
    - [x] Implementierung der Kern-Klassen für physische Ausgänge und logische Funktionen (`PhysicalOutputManager`, `LogicalFunction`, `FunctionManager`).
    - [x] Implementierung eines CV-basierten Lade-Mechanismus (`CVLoader`, `CVManager`).
- [x] **2. Erweiterte Effekte & Mapping**
    - [x] Implementierung der vollständigen, mehrstufigen Tabellenlogik für das Funktions-Mapping (CV-basiert, `CVLoader`).
    - [x] Implementierung aller im Konzept (`LIGHT_AND_AUX_CONCEPT.MD`) vorgesehenen Lichteffekte (`Steady`, `Dimming`, `Flicker`, `Strobe`, `MarsLight`, `SoftStartStop`).
    - [x] Entwicklung einer umfassenden CV-Tabelle zur Konfiguration (siehe `CV_MAPPING.md`).
- [x] **3. Hilfsfunktionen**
    - [x] Implementierung des `SERVO_CONTROL`-Effekts.
    - [x] Implementierung des `SMOKE_GENERATOR`-Effekts.
- [ ] **4. Werkzeuge & Verfeinerung**
    - [ ] Entwicklung eines PC-basierten Konfigurationstools.
    - [ ] Erstellung einer umfassenden Benutzerdokumentation.

## Phase 4: Implementierung des Sound-Systems

- [ ] **1. Kern-Audio-Infrastruktur**
    - [ ] Implementierung der grundlegenden Hardware-Treiber (I2S, DMA) und eines WAV-Readers.
- [ ] **2. Polyphonie & Prime Mover**
    - [ ] Implementierung eines Software-Mixers für die gleichzeitige Wiedergabe mehrerer Sounds.
    - [ ] Implementierung eines dynamischen `PRIME_MOVER`-Sounds.
- [ ] **3. Erweitertes Mapping & Spezial-Lokomotiven**
    - [ ] Implementierung der vollständigen Tabellenlogik für das flexible Sound-Mapping.
    - [ ] Implementierung der `PRIME_MOVER`-Logik für Dampf- und Elektro-Lokomotiven.
- [ ] **4. Werkzeuge & "Sound-Font"-Verwaltung**
    - [ ] Entwicklung eines PC-basierten Tools zur Erstellung und Verwaltung von "Sound-Fonts".

## Phase 5: Hardware-Entwicklung

- [ ] **1. Anforderungsanalyse & Design**
    - [ ] Definition der Hardware-Anforderungen (Größe, Leistung, Anschlüsse).
    - [ ] Auswahl der Kernkomponenten (H-Brücke, Spannungsregler, etc.).
- [ ] **2. Schaltplan & Layout**
    - [ ] Erstellung des Schaltplans in einem EDA-Tool (z.B. KiCad).
    - [ ] Design des Platinen-Layouts unter Berücksichtigung der Einbaumaße.
- [ ] **3. Prototyping & Test**
    - [ ] Bestellung von Prototypen-Platinen.
    - [ ] Bestückung und Inbetriebnahme der ersten Prototypen.
    - [ ] Durchführung von elektrischen Tests und Validierung der Funktionalität.
- [ ] **4. Dokumentation**
    - [ ] Erstellung von Bestückungsplänen und Stücklisten (BOM).
    - [ ] Veröffentlichung der Hardware-Designdateien im `hardware/`-Verzeichnis.

## Zukünftige Ideen

- [ ] RailCom-Unterstützung
- [ ] LCC/Layout Command Control Integration
- [ ] Unterstützung für weitere Motortypen
- [ ] Drahtlose Steuerung (z.B. Wi-Fi, Bluetooth)
