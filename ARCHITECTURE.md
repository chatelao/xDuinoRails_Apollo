# Architecture of xDuinoRails_Apollo

This document provides a high-level technical overview of the software and hardware architecture of the **xDuinoRails_Apollo** multi-protocol locomotive decoder.

## 1. Hardware Platform

The project is designed around the **Seeed Studio XIAO RP2040** microcontroller.

*   **Microcontroller**: RP2040 (Dual-core ARM Cortex-M0+ @ 133MHz).
*   **Key Peripherals Used**:
    *   **PIO (Programmable I/O)**: Drives the I2S interface for high-fidelity audio.
    *   **PWM**: Controls motor speed and lighting effects.
    *   **DMA**: Offloads audio buffer transfers from the CPU.
    *   **Flash**: Stores the firmware and the virtual sound decoder (VSD) files.
*   **I/O Mapping**: Defined in `firmware/src/config.h`.

## 2. Firmware Architecture

The firmware is developed using **PlatformIO** and the **Arduino framework** (specifically the `arduino-pico` core). The architecture is modular, with distinct responsibilities encapsulated in separate libraries and managers.

### 2.1. Core Modules

*   **Protocol Manager**:
    *   Responsibility: Runtime detection of the active digital protocol (DCC or Märklin-Motorola).
    *   Libraries: `NmraDcc` (for DCC decoding) and `MaerklinMotorola` (for MM decoding).
    *   Mechanism: Monitors the track signal via interrupts and delegates packet processing to the appropriate library.

*   **Motor Control** (`xDuinoRails_MotorControl`):
    *   Responsibility: Closed-loop speed control of the locomotive motor.
    *   Features: PID control loop, Back-EMF (BEMF) sensing, load compensation.
    *   Configuration: Standard DCC Configuration Variables (CVs).

*   **Lights & Auxiliary Functions** (`xDuinoRails_DccLightsAndFunctions`):
    *   Responsibility: Managing all physical outputs (lights, servos, smoke generators) and their behaviors.
    *   **AuxController**: The central orchestrator that updates logical functions and effects.
    *   **Function Mapping**: Implements a powerful "Multi-Level Table Logic" engine, configurable via the **RCN-227** standard. It maps triggers (Function Keys, Decoder State) to Logical Functions.
    *   **Effects**: A library of visual effects (e.g., `EffectFlicker`, `EffectMarsLight`) that manipulate the PWM outputs.

*   **Sound System**:
    *   Responsibility: Polyphonic audio playback based on the JMRI Virtual Sound Decoder (VSD) format.
    *   **VSDReader**: Parses `.vsd` files (ZIP archives containing XML and WAVs) using the `miniz` (decompression) and `expat` (XML parsing) libraries.
    *   **SoftwareMixer**: Mixes multiple audio streams (`WAVStream`) into a single stereo output.
    *   **I2SDriver**: Handles the low-level transmission of audio data to the DAC via I2S.

*   **CV Manager** (`xDuinoRails_CVManager`):
    *   Responsibility: Centralized management of Configuration Variables.
    *   Features: Handles storage persistence (Flash simulation of EEPROM) and provides the logic for indexed CV access required by RCN-227.

### 2.2. Project Structure

*   **`firmware/src`**: Contains the main application logic, protocol integration, and hardware abstraction layer (HAL) implementations.
*   **`firmware/lib`**: Contains vendored dependencies (`miniz`, `expat`) and project-specific libraries (`xDuinoRails_...`).
*   **`firmware/test`**: Contains the unit test suite, utilizing the Unity framework.

## 3. Data Flow

The system operates on a main loop with interrupt-driven input handling:

1.  **Input Stage (Interrupt Context)**:
    *   The track signal triggers an interrupt.
    *   `ProtocolManager` captures the edge timing.
    *   `NmraDcc` or `MaerklinMotorola` decodes the bitstream into a packet.

2.  **Processing Stage (Main Loop)**:
    *   **Packet Handling**: Valid packets result in commands (e.g., "Set Speed 50", "Toggle F1").
    *   **State Update**:
        *   **Motor**: The PID loop calculates the new PWM duty cycle based on target speed and BEMF feedback.
        *   **Functions**: The `FunctionManager` evaluates the current state (Function Keys + Direction + Speed) against the mapping rules. Active rules trigger their associated `LogicalFunction`.
        *   **Sound**: Triggers from the mapping system (or direct events) start/stop `WAVStreams` in the `SoftwareMixer`.

3.  **Output Stage**:
    *   **PWM**: Updated values are written to the motor and light output pins.
    *   **Audio**: The `SoftwareMixer` fills the DMA buffer, which is automatically pushed to the I2S interface.

## 4. Design Principles

*   **Separation of Concerns**: Logical behaviors (e.g., "Headlight") are strictly separated from physical hardware implementation (e.g., "Pin 5"). This allows for flexible remapping.
*   **Data-Driven Configuration**: The decoder's behavior is largely defined by data—CVs for mapping and motor control, and XML/VSD files for sound—rather than hard-coded logic.
*   **Standards Compliance**: The architecture is built to support RailCommunity (RCN) standards, particularly RCN-225 and RCN-227 for function mapping.
