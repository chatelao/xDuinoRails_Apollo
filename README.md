# xDuinoRails_Apollo: A Multi-Protocol Locomotive Decoder

This repository contains the documentation, schematics, and source code for the xDuinoRails_Apollo, a multi-protocol (DCC and Märklin-Motorola) locomotive decoder.

## Features

*   **Multi-Protocol Support:** Supports both DCC and Märklin-Motorola (MM) protocols.
*   **Advanced Motor Control:** Utilizes the `xDuinoRails_MotorControl` library for advanced, closed-loop motor control.
*   **Flexible Function Mapping:** The `FunctionManager` allows for declarative mapping of function keys to physical outputs and behaviors.
*   **Modern Hardware:** The decoder is designed for the Seeed Studio XIAO RP2040 microcontroller.

## Architecture

![Architecture Diagram](architecture/architecture.png)

## Repository Structure

*   `docs/`: Contains all project documentation, including concepts for the sound and light systems.
*   `firmware/`: Contains the PlatformIO project for the decoder's firmware.

## Getting Started

### Firmware

The firmware is a PlatformIO project. To build the firmware, you will need to have PlatformIO installed.

1.  **Install PlatformIO:** If you don't have it installed, you can install it via pip:
    ```bash
    pip install platformio
    ```
2.  **Build the firmware:** You can build the firmware for a specific protocol using the following command:
    ```bash
    # For DCC
    python -m platformio run -d firmware -e xiao_dcc

    # For Märklin-Motorola
    python -m platformio run -d firmware -e xiao_mm
    ```

## Long-Term Vision

This project has a long-term vision that includes:

*   A state-of-the-art, polyphonic sound system.
*   A state-of-the-art light and auxiliary function management system.
*   RailCom support.
