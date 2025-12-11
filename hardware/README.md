# Hardware Revision 2 (RP2040)

This directory contains the KiCad schematic for the RP2040-based decoder hardware, designed for KiCad Version 9.

## Seeed XIAO RP2040 is used in the 1st Gen decoders

<img width="350" alt="image" src="https://github.com/user-attachments/assets/27688da3-2d20-4c41-9307-29afc06e1df2" />

### Revision 1 - Oct 2025 - XDR Fortuna with BDR6133 


The pin assignment corresponds to the configuration in `firmware/src/config.h` (RP2040 version) and targets the **Seeed Xiao RP2040** module.

| Function | Pin Name | GPIO | Description |
|----------|----------|------|-------------|
| BEMF B | D2 / A2 | 28 | Back-EMF Measurement Input B |
| BEMF A | D3 / A3 | 29 | Back-EMF Measurement Input A |
| Motor A | D7 | 0 | Motor Output A (PWM to BDR6133) |
| Motor B | D8 | 1 | Motor Output B (PWM to BDR6133) |

### Revision 2 - XDR Thor with BDR6133 

The pin assignment corresponds to the configuration in `firmware/src/config.h` (RP2040 version) and targets the **Seeed Xiao RP2040** module.

| Function | Pin Name | GPIO | Description |
|----------|----------|------|-------------|
| Headlight Fwd | D0 / A0 | 26 | Front Headlight (PWM capable) |
| Headlight Rev | D1 / A1 | 27 | Rear Headlight (PWM capable) |
| BEMF B | D2 / A2 | 28 | Back-EMF Measurement Input B |
| BEMF A | D3 / A3 | 29 | Back-EMF Measurement Input A |
| PWM Sound | D4 | 6 | PWM Audio Output (Alternative to I2S) |
| Unused | D5 | 7 | (Available) |
| Motor A | D7 | 0 | Motor Output A (PWM) |
| Motor B | D7 | 1 | Motor Output B (PWM) |
| I2S BCLK | D8 | 2 | I2S Bit Clock |
| I2S DIN | D9 | 4 | I2S Data In |
| I2S LRCLK | D10 | 3 | I2S Left/Right Clock |
