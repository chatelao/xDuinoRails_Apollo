# CV-Tabelle für den xDuino-Lokdecoder

Dieses Dokument beschreibt die Konfigurationsvariablen (CVs) für den xDuino-Lokdecoder. Die CVs ermöglichen die Anpassung der Fahreigenschaften, der Adresse und weiterer Funktionen des Decoders.

Die Struktur orientiert sich an gängigen Industriestandards (z.B. ESU, ZIMO), um eine intuitive Bedienung mit den meisten digitalen Zentralen zu gewährleisten.

## CV-Übersicht

| CV | Name | Beschreibung | Wertebereich | Standardwert | Status |
|----|------|--------------|--------------|--------------|--------|
| **1** | **Primäre Adresse** | Die kurze DCC-Adresse der Lokomotive. | 1-127 | `3` | **Implementiert** |
| 2 | Startspannung | Definiert die minimale Spannung (PWM-Wert), die an den Motor gesendet wird. Hilfreich für ein sanftes Anfahren. | 0-255 | `80` | **Implementiert** |
| 3 | Beschleunigungszeit | Definiert die Zeit, die die Lokomotive benötigt, um von Geschwindigkeit 0 auf die Höchstgeschwindigkeit zu beschleunigen. Der Wert ist ein Multiplikator (z.B. Wert * 0.25s). | 0-255 | `20` (`50` in `config.h`) | **Implementiert** |
| 4 | Bremszeit | Definiert die Zeit, die die Lokomotive benötigt, um von Höchstgeschwindigkeit auf 0 abzubremsen. Der Wert ist ein Multiplikator (z.B. Wert * 0.25s). | 0-255 | `40` (`100` in `config.h`) | **Implementiert** |
| 5 | Maximale Geschwindigkeit | Begrenzt die Höchstgeschwindigkeit der Lokomotive. | 0-255 | `255` | Nicht implementiert |
| 6 | Mittlere Geschwindigkeit | Definiert einen Punkt in der Mitte der Geschwindigkeitskurve für eine nicht-lineare Kennlinie. | 0-255 | `128` | Nicht implementiert |
| 7 | Hersteller-Version | Versionsnummer der Decoder-Firmware. | Lesen | `1` | Nicht implementiert |
| **8** | **Hersteller-ID** | Kennung des Herstellers (NMRA Standard). | Lesen | `165` (DIY) | **Implementiert** |
| | | | | | |
| 17 | Erweiterte Adresse (High Byte) | Höherwertiges Byte für lange DCC-Adressen (128-10239). | 192-231 | `192` | Nicht implementiert |
| 18 | Erweiterte Adresse (Low Byte) | Niederwertiges Byte für lange DCC-Adressen. | 0-255 | `3` | Nicht implementiert |
| | | | | | |
| 29 | Konfigurationsdaten 1 | Bit-Feld für grundlegende Einstellungen (Fahrtrichtung, Geschwindigkeitsstufen, Adressmodus). | Bit-Feld | `6` | Nicht implementiert |
| | | | | | |
| 49 | Konfiguration Funktionsausgang 1 (Licht vorne) | Mapping und Effekte für den Lichtausgang vorne (F0f). | Bit-Feld | `tbd` | Nicht implementiert |
| 50 | Konfiguration Funktionsausgang 2 (Licht hinten) | Mapping und Effekte für den Lichtausgang hinten (F0r). | Bit-Feld | `tbd` | Nicht implementiert |

---

### Hinweise zu den Werten

*   **Beschleunigungs-/Bremszeit (CV 3/4):** Die aktuellen Werte in `config.h` sind `50` und `100` (Einheit: `PPS^2`). Um dies auf den NMRA-Standard (Wert * 0.25s) abzubilden, müssen die Werte in der Firmware umgerechnet werden. Die Standardwerte in der Tabelle (`20` und `40`) sind Annäherungen und können angepasst werden.
*   **Hersteller-ID (CV 8):** Der Wert `165` ist im NMRA-Verzeichnis für "DIY and home-built decoders" reserviert und wird in der Firmware bereits verwendet.
*   **Erweiterte Adressen (CV 17/18):** Die Implementierung erfordert die Auswertung von CV 29, um zwischen kurzer und langer Adresse umzuschalten.
*   **Funktionsausgänge (CV 49/50):** Diese CVs sind für einfache Decoder gedacht. Das hier implementierte erweiterte "Function Mapping" (siehe unten) macht sie weitgehend überflüssig. Sie werden vorerst nicht implementiert.

---

## Konfiguration des Erweiterten Funktions-Mappings

Das erweiterte Funktions-Mapping ist ein extrem flexibles System, das auf drei Hauptkomponenten basiert: **Logische Funktionen**, **Bedingungsvariablen** und **Mapping-Regeln**. Diese werden über dedizierte CV-Bereiche konfiguriert.

### 1. Logische Funktionen (CV 200-455) - **IMPLEMENTIERT**

Die logischen Funktionen definieren, *was* der Decoder tut (z.B. ein Licht einschalten, ein Servo bewegen). Es können bis zu 32 logische Funktionen konfiguriert werden. Jede Funktion belegt einen Block von 8 CVs.

- **Basis-CV:** `200`
- **Anzahl Blöcke:** `32`
- **Blockgröße:** `8 CVs`

| CV (Basis + Offset) | Name | Beschreibung | Wertebereich |
|---------------------|------|--------------|--------------|
| **Base + 0** | **Effekt-Typ** | Definiert das Verhalten der Funktion. | 0-255 |
| Base + 1 | Parameter 1 | Erster Parameter für den gewählten Effekt. | 0-255 |
| Base + 2 | Parameter 2 | Zweiter Parameter für den gewählten Effekt. | 0-255 |
| Base + 3 | Parameter 3 | Dritter Parameter für den gewählten Effekt. | 0-255 |
| Base + 4 | Physischer Ausgang 1 ID | ID des ersten zugeordneten Ausgangs (0-basiert). | 0-255 |
| Base + 5 | Physischer Ausgang 2 ID | ID des zweiten Ausgangs (z.B. für Rauchgenerator-Lüfter). | 0-255 |
| Base + 6 | (Reserviert) | | |
| Base + 7 | (Reserviert) | | |

### Effekt-Typen und ihre Parameter

| Typ-ID | Effekt | Parameter 1 | Parameter 2 | Parameter 3 |
|--------|--------|---------------|---------------|---------------|
| 1 | **Dauerlicht** (Steady) | Helligkeit | - | - |
| 2 | **Dimmen** (Dimming) | Helligkeit (voll) | Helligkeit (gedimmt) | - |
| 3 | **Flackern** (Flicker) | Basish Helligkeit | Flackertiefe | Flackergeschwindigkeit |
| 4 | **Stroboskop** (Strobe) | Frequenz (Hz) | Einschaltdauer (%) | Helligkeit |
| 5 | **Mars Light** | Frequenz (mHz) | Spitzenhelligkeit | Phasenverschiebung (%) |
| 6 | **Sanftes Ein-/Ausschalten** | Einblendzeit (ms/2) | Ausblendzeit (ms/2) | Ziel-Helligkeit |
| 7 | **Servo** | Endpunkt A (Grad) | Endpunkt B (Grad) | Bewegungsgeschwindigkeit |
| 8 | **Rauchgenerator** | Heizung an/aus (0/1) | Lüftergeschwindigkeit | - |

---

### 2. Bedingungsvariablen (CV 500-627) - **IMPLEMENTIERT**

Die Bedingungsvariablen definieren, *wann* etwas passieren soll. Sie prüfen den Zustand des Decoders (z.B. "Fährt der Zug vorwärts?", "Ist F1 eingeschaltet?"). Es können bis zu 32 Bedingungsvariablen konfiguriert werden. Jede Variable belegt einen Block von 4 CVs.

- **Basis-CV:** `500`
- **Anzahl Blocke:** `32`
- **Blockgröße:** `4 CVs`

| CV (Basis + Offset) | Name | Beschreibung | Wertebereich |
|---------------------|------|--------------|--------------|
| **Base + 0** | **Quelle (Source)** | Die Datenquelle, die geprüft werden soll. | `1-4` |
| Base + 1 | **Vergleichsoperator (Comparator)** | Wie die Quelle geprüft werden soll. | `1-8` |
| Base + 2 | **Parameter** | Der Wert, mit dem verglichen wird. | `0-255` |
| Base + 3 | (Reserviert) | | |

#### Quellen (Source)

| ID | Quelle | Beschreibung |
|----|--------|--------------|
| 1 | **Function Key** | Zustand einer Funktionstaste (F0-F28). Parameter = Tasten-Nummer. |
| 2 | **Direction** | Fahrtrichtung des Decoders. |
| 3 | **Speed** | Geschwindigkeit des Decoders. |
| 4 | **Logical Function State** | Zustand einer anderen logischen Funktion. Parameter = LF ID (1-32). |

#### Vergleichsoperatoren (Comparator)

| ID | Operator | Beschreibung |
|----|----------|--------------|
| 1 | `EQ` (Equal) | Ist gleich |
| 2 | `NEQ` (Not Equal) | Ist ungleich |
| 3 | `GT` (Greater Than) | Ist größer als |
| 4 | `LT` (Less Than) | Ist kleiner als |
| 5 | `GTE` (Greater Than or Equal) | Ist größer oder gleich |
| 6 | `LTE` (Less Than or Equal) | Ist kleiner oder gleich |
| 7 | `BIT_AND` | Bit-weises UND ist nicht null |
| 8 | `IS_TRUE` | Ist wahr (Wert != 0) |

---

### 3. Mapping-Regeln (CV 700-955) - **IMPLEMENTIERT**

Die Mapping-Regeln sind das Herzstück des Systems. Sie verknüpfen die **Bedingungsvariablen** mit den **Logischen Funktionen**. Eine Regel lautet z.B.: "Wenn Bedingung 1 wahr ist UND Bedingung 2 nicht wahr ist, DANN schalte die logische Funktion 5 ein." Es können bis zu 64 Regeln konfiguriert werden. Jede Regel belegt einen Block von 4 CVs.

- **Basis-CV:** `700`
- **Anzahl Blöcke:** `64`
- **Blockgröße:** `4 CVs`

| CV (Basis + Offset) | Name | Beschreibung | Wertebereich |
|---------------------|------|--------------|--------------|
| **Base + 0** | **Ziel Logische Funktion ID** | ID der zu steuernden logischen Funktion (1-32). | `1-32` |
| Base + 1 | **Positive Bedingung ID** | ID der Bedingungsvariable (1-32), die WAHR sein muss. 0 = ignoriert. | `0-32` |
| Base + 2 | **Negative Bedingung ID** | ID der Bedingungsvariable (1-32), die FALSCH sein muss. 0 = ignoriert. | `0-32` |
| Base + 3 | **Aktion (Action)** | Was mit der logischen Funktion geschehen soll. | `1-3` |

#### Aktionen (Action)

| ID | Aktion | Beschreibung |
|----|--------|--------------|
| 1 | `ACTIVATE` | Aktiviert die logische Funktion. |
| 2 | `DEACTIVATE` | Deaktiviert die logische Funktion. |
| 3 | `SET_DIMMED` | Setzt den "dimmed" Zustand der Funktion (nur für `EffectDimming`). |
