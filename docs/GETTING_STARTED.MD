# Getting Started with the RP2040 DCC Locomotive Decoder

This guide will help you set up the hardware, flash the firmware, and use the Minimum Viable Product (MVP) features of the RP2040 DCC Locomotive Decoder.

## Hardware Setup

1.  **Gather Your Components:**
    *   XIAO RP2040
    *   BDR-6133 Motor Driver
    *   A DCC interface board similar to the `xDuinoRails_Hermes-M`
    *   A Märklin motor
    *   A 12V DC power supply for the DCC signal
    *   A 5V power supply for the XIAO RP2040

2.  **Wiring:**
    *   Connect the DCC signal from your command station to the DCC input of the `Hermes-M` board.
    *   Connect the 5V power supply to the 5V and GND pins of the XIAO RP2040.
    *   Connect the XIAO RP2040 to the `Hermes-M` and the BDR-6133 motor driver according to the wiring diagram in the `hardware/` directory.
    *   Connect the motor to the `OutA` and `OutB` terminals of the BDR-6133.
    *   Connect a headlight (or an LED with a current-limiting resistor) to pin `D0` and `GND` of the XIAO RP2040.

## Flashing the Firmware

1.  **Install PlatformIO:** If you haven't already, [install PlatformIO IDE for VS Code](https://platformio.org/install/ide?install=vscode).
2.  **Open the Project:** Open this project folder in VS Code.
3.  **Build and Upload:**
    *   Connect the XIAO RP2040 to your computer via USB.
    *   Click the "Upload" button in the PlatformIO toolbar at the bottom of the VS Code window.

## Using the MVP Features

1.  **DCC Address:** The decoder is hardcoded to DCC address `3`.
2.  **Motor Control:**
    *   Use your DCC command station to select address `3`.
    *   Use the throttle to control the motor's speed and direction.
3.  **Headlight Control:**
    *   Use your DCC command station to control function `F0`. This will turn the headlight on and off.
