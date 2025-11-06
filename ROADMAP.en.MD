# Project Roadmap

This document outlines the necessary steps to develop a fully functional multi-protocol locomotive decoder based on the Seeed Studio XIAO RP2040.

## Phase 1: Minimal Viable Product (MVP)

- [x] **1. Project Setup and Library Integration**
    - [x] Integrate the `LaserLight/MaerklinMotorola` library as a Git submodule or via the PlatformIO Library Manager.
    - [x] Implement a switch between DCC and MM protocols using compiler flags.
    - [x] Ensure that the code can be compiled separately for both protocols.
- [x] **2. Implementation of Motor Control (Open-Loop PWM)**
    - [x] Define the hardware pins for the H-bridge control on the XIAO RP2040.
    - [x] Implement simple PWM functionality to regulate the motor's speed.
    - [x] Read the speed and direction commands from the `MaerklinMotorola` library and pass them to the PWM and direction logic.
- [x] **3. Adapt GitHub Actions Workflow**
    - [x] Create two separate build jobs: one for the DCC version (`pio run -e xiao_dcc`) and one for the MM version (`pio run -e xiao_mm`).
    - [x] Save the compiled firmware files of both jobs as separate "Release Assets" or build artifacts.

## Phase 2: Feature Enhancements

- [x] **1. Advanced Motor Control**
    - [x] Integrate the `chatelao/xDuinoRails_MotorControl` library into the project.
    - [x] Replace the simple PWM control with advanced regulation (e.g., PID control for load compensation).
- [x] **2. Function Outputs**
    - [x] Implement control for at least one function output (e.g., for lights).
    - [x] Evaluate the function commands (F0, F1, etc.) from the `MaerklinMotorola` library.
- [ ] **3. Multi-protocol Capability (Simultaneous Operation)**
    - [ ] Extend the decoder to automatically distinguish between DCC and MM signals at runtime.
    - [ ] Create a common interface for motor and function control that can be used by both protocol implementations.
- [ ] **4. Configurability**
    - [ ] Create a way to program the decoder's MM address.
    - [ ] Add configurable parameters such as acceleration and braking delay (ABV).

## Phase 3: Implementation of Light and Aux Functions

- [ ] **1. Core Infrastructure**
    - [ ] Implement the core classes for Physical Outputs and Logical Functions.
    - [ ] Implement the `LightEffect` base class and simple effects.
- [ ] **2. Advanced Effects & Mapping**
    - [ ] Implement the full, multi-level table logic for function mapping.
    - [ ] Implement all remaining advanced lighting effects.
    - [ ] Develop a comprehensive CV table for configuration.
- [ ] **3. Auxiliary Functions**
    - [ ] Implement the `SERVO_CONTROL` effect.
    - [ ] Implement the `SMOKE_GENERATOR` effect.
- [ ] **4. Tooling & Refinement**
    - [ ] Develop a PC-based configuration tool.
    - [ ] Write comprehensive user documentation.

## Phase 4: Sound System Implementation

- [ ] **1. Core Audio Infrastructure**
    - [ ] Implement the basic hardware drivers (I2S, DMA) and a WAV reader.
- [ ] **2. Polyphony & Prime Mover**
    - [ ] Implement a software mixer for simultaneous playback of multiple sounds.
    - [ ] Implement a dynamic `PRIME_MOVER` sound.
- [ ] **3. Advanced Mapping & Special Locomotives**
    - [ ] Implement the full table logic for flexible sound mapping.
    - [ ] Implement the `PRIME_MOVER` logic for steam and electric locomotives.
- [ ] **4. Tooling & "Sound-Font" Management**
    - [ ] Develop a PC-based tool for creating and managing "sound-fonts".

## Phase 5: Hardware Development

- [ ] **1. Requirement Analysis & Design**
    - [ ] Define hardware requirements (size, power, connectors).
    - [ ] Select core components (H-bridge, voltage regulator, etc.).
- [ ] **2. Schematics & Layout**
    - [ ] Create schematics in an EDA tool (e.g., KiCad).
    - [ ] Design the PCB layout, considering installation dimensions.
- [ ] **3. Prototyping & Testing**
    - [ ] Order prototype PCBs.
    - [ ] Assemble and commission the first prototypes.
    - [ ] Conduct electrical tests and validate functionality.
- [ ] **4. Documentation**
    - [ ] Create assembly diagrams and a Bill of Materials (BOM).
    - [ ] Publish the hardware design files in the `hardware/` directory.

## Future Ideas

- [ ] RailCom Support
- [ ] LCC/Layout Command Control Integration
- [ ] Support for other motor types
- [ ] Wireless control (e.g., Wi-Fi, Bluetooth)
