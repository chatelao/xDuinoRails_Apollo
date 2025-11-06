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
*   **Funktionsausgänge (CV 49/50):** Die Planung für ein erweitertes "Function Mapping" ist im `light_and_aux_concept.md` vorgesehen. Die CVs sind hier als Platzhalter reserviert.
