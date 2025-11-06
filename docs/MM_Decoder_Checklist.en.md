# Checklist for the Märklin-Motorola (MM) Decoder

This document describes the necessary steps to develop a fully functional Märklin-Motorola decoder based on the Seeed Studio XIAO RP2040.

## Phase 1: Minimal Viable Product (MVP) - MM Functionality

The goal of this phase is to create a decoder that can control a locomotive using the Märklin-Motorola protocol (driving only).

- [x] **1. Project Setup and Library Integration**
    - [x] Integrate the `LaserLight/MaerklinMotorola` library as a Git submodule or via the PlatformIO Library Manager.
    - [x] Implement a switch between DCC and MM protocols using compiler flags.
        - [x] **Suggestion:** Define two environments in `platformio.ini`: `[env:xiao_dcc]` and `[env:xiao_mm]`.
        - [x] **Suggestion:** Set build flags `-DPROTOCOL_DCC` and `-DPROTOCOL_MM` in the respective environments.
    - [x] Ensure that the code can be compiled separately for both protocols by wrapping the protocol-specific logic with `#ifdef PROTOCOL_DCC` / `#endif` and `#ifdef PROTOCOL_MM` / `#endif`.
    - [x] Only include and initialize the `NmraDcc` library if `PROTOCOL_DCC` is defined.
    - [x] Only include and initialize the `MaerklinMotorola` library if `PROTOCOL_MM` is defined.

- [x] **2. Implementation of Motor Control (Open-Loop PWM)**
    - [x] Define the hardware pins for the H-bridge control on the XIAO RP2040.
    - [x] Implement simple PWM functionality to regulate the motor's speed.
    - [x] Create a function to control the direction of travel by reversing the polarity of the H-bridge.
    - [x] Read the speed and direction commands from the `MaerklinMotorola` library and pass them to the PWM and direction logic.

- [x] **3. Adapt GitHub Actions Workflow**
    - [x] Extend the `build.yml` workflow file.
    - [x] Create two separate build jobs: one for the DCC version (`pio run -e xiao_dcc`) and one for the MM version (`pio run -e xiao_mm`).
    - [x] Save the compiled firmware files (`firmware.bin` or `firmware.uf2`) of both jobs as separate "Release Assets" or build artifacts (e.g., `firmware_dcc.bin` and `firmware_mm.bin`).

## Phase 2: Future Extensions

- [x] **1. Advanced Motor Control**
    - [x] Integrate the `chatelao/xDuinoRails_MotorControl` library into the project.
    - [x] Replace the simple PWM control with the advanced regulation (e.g., PID control for load compensation) from the new library.

- [x] **2. Function Outputs**
    - [x] Implement control for at least one function output (e.g., for lights).
    - [x] Evaluate the function commands (F0, F1, etc.) from the `MaerklinMotorola` library.

- [ ] **3. Multi-protocol Capability (Simultaneous Operation)**
    - [ ] Extend the decoder to automatically distinguish between DCC and MM signals at runtime.
    - [ ] Create a common interface for motor and function control that can be used by both protocol implementations.

- [ ] **4. Configurability**
    - [ ] Create a way to program the decoder's MM address (e.g., via a "programming mode").
    - [ ] Add configurable parameters such as acceleration and braking delay (ABV).

## Phase 3: Implementation of Light and Aux Functions

- [ ] **1. Core Infrastructure**
    - [ ] Implement the core classes for Physical Outputs and Logical Functions.
    - [ ] Implement the `LightEffect` base class and the simplest effects: `Steady` and `Dimming`.
    - [ ] Implement a basic, direct mapping system to allow F-keys to turn logical functions on and off.
- [ ] **2. Advanced Effects & Mapping**
    - [ ] Implement the full, multi-level table logic for function mapping.
    - [ ] Implement all remaining advanced lighting effects (Flicker, Strobe, Mars Light, etc.).
    - [ ] Develop a comprehensive CV table to configure all new features.
- [ ] **3. Auxiliary Functions**
    - [ ] Implement the `SERVO_CONTROL` effect.
    - [ ] Implement the `SMOKE_GENERATOR` effect, including speed-synchronized fan control.
    - [ ] Add CVs for configuring these new auxiliary functions.
- [ ] **4. Tooling & Refinement**
    - [ ] Develop a PC-based configuration tool to simplify the CV programming process.
    - [ ] Write comprehensive user documentation for all features.
    - [ ] Refine and optimize the code based on community feedback.
