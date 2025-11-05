# Checkliste für den Märklin-Motorola (MM) Decoder

Dieses Dokument beschreibt die notwendigen Schritte, um einen voll funktionsfähigen Märklin-Motorola-Decoder auf Basis des Seeed Studio XIAO RP2040 zu entwickeln.

## Phase 1: Minimal Viable Product (MVP) - MM-Funktionalität

Ziel dieser Phase ist es, einen Decoder zu erstellen, der eine Lok mit dem Märklin-Motorola-Protokoll steuern kann (nur Fahren).

- [ ] **1. Projekt-Setup und Bibliotheks-Integration**
    - [ ] Die `LaserLight/MaerklinMotorola` Bibliothek als Git-Submodul oder über den PlatformIO Library Manager in das Projekt integrieren.
    - [ ] Eine Umschaltmöglichkeit zwischen DCC- und MM-Protokoll mittels Compiler-Flags implementieren.
        - **Vorschlag:** In `platformio.ini` zwei Umgebungen definieren: `[env:xiao_dcc]` und `[env:xiao_mm]`.
        - **Vorschlag:** Build-Flags `-DPROTOCOL_DCC` und `-DPROTOCOL_MM` in den jeweiligen Umgebungen setzen.
    - [ ] Sicherstellen, dass der Code für beide Protokolle separat kompiliert werden kann, indem die protokollspezifische Logik mit `#ifdef PROTOCOL_DCC` / `#endif` und `#ifdef PROTOCOL_MM` / `#endif` umschlossen wird.
    - [ ] Die `NmraDcc` Bibliothek nur dann einbinden und initialisieren, wenn `PROTOCOL_DCC` definiert ist.
    - [ ] Die `MaerklinMotorola` Bibliothek nur dann einbinden und initialisieren, wenn `PROTOCOL_MM` definiert ist.

- [ ] **2. Implementierung der Motorsteuerung (Open-Loop PWM)**
    - [ ] Die Hardware-Pins für die H-Brücken-Ansteuerung am XIAO RP2040 definieren.
    - [ ] Eine einfache PWM-Funktionalität implementieren, um die Geschwindigkeit des Motors zu regeln.
    - [ ] Eine Funktion erstellen, die die Fahrtrichtung durch Umpolen der H-Brücke steuert.
    - [ ] Die Geschwindigkeits- und Richtungsbefehle aus der `MaerklinMotorola` Bibliothek auslesen und an die PWM- und Richtungs-Logik weitergeben.

- [ ] **3. GitHub Actions Workflow anpassen**
    - [ ] Die `build.yml` Workflow-Datei erweitern.
    - [ ] Zwei separate Build-Jobs erstellen: einen für die DCC-Version (`pio run -e xiao_dcc`) und einen für die MM-Version (`pio run -e xiao_mm`).
    - [ ] Die kompilierten Firmware-Dateien (`firmware.bin` oder `firmware.uf2`) beider Jobs als separate "Release Assets" oder Build-Artefakte speichern (z.B. `firmware_dcc.bin` und `firmware_mm.bin`).

## Phase 2: Zukünftige Erweiterungen

- [ ] **1. Fortgeschrittene Motorsteuerung**
    - [ ] Die `chatelao/xDuinoRails_MotorControl` Bibliothek in das Projekt integrieren.
    - [ ] Die einfache PWM-Steuerung durch die fortgeschrittene Regelung (z.B. PID-Regelung für Lastausgleich) aus der neuen Bibliothek ersetzen.

- [ ] **2. Funktionsausgänge**
    - [ ] Ansteuerung für mindestens einen Funktionsausgang (z.B. für Licht) implementieren.
    - [ ] Die Funktionsbefehle (F0, F1, etc.) aus der `MaerklinMotorola` Bibliothek auswerten.

- [ ] **3. Multiprotokoll-Fähigkeit (Gleichzeitiger Betrieb)**
    - [ ] Den Decoder so erweitern, dass er zur Laufzeit automatisch zwischen DCC- und MM-Signalen unterscheiden kann.
    - [ ] Eine gemeinsame Schnittstelle für die Motor- und Funktionssteuerung schaffen, die von beiden Protokoll-Implementierungen genutzt werden kann.

- [ ] **4. Konfigurierbarkeit**
    - [ ] Eine Möglichkeit schaffen, die MM-Adresse des Decoders zu programmieren (z.B. über einen "Programmiermodus").
    - [ ] Konfigurierbare Parameter wie Anfahr- und Bremsverzögerung (ABV) hinzufügen.
