# Project Roadmap

This document describes the necessary steps to develop a fully functional multi-protocol locomotive decoder based on the Seeed Studio XIAO RP2040.

## Phase 1: Minimal Viable Product (MVP)

- [x] **1. Project Setup and Library Integration**
    - [x] Integrate the `LaserLight/MaerklinMotorola` library as a Git submodule or via the PlatformIO Library Manager.
    - [x] Implement a switch between DCC and MM protocols using compiler flags.
    - [x] Ensure that the code can be compiled separately for both protocols.
- [x] **2. Motor Control Implementation (Open-Loop PWM)**
    - [x] Define the hardware pins for the H-bridge control on the XIAO RP2040.
    - [x] Implement simple PWM functionality to regulate the motor's speed.
    - [x] Read speed and direction commands from the `MaerklinMotorola` library and pass them to the PWM and direction logic.
- [x] **3. GitHub Actions Workflow Adjustment**
    - [x] Create two separate build jobs: one for the DCC version (`pio run -e xiao_dcc`) and one for the MM version (`pio run -e xiao_mm`).
    - [x] Save the compiled firmware files from both jobs as separate "Release Assets" or build artifacts.

## Phase 2: Feature Enhancements

- [x] **1. Advanced Motor Control**
    - [x] Integrate the `chatelao/xDuinoRails_MotorControl` library into the project.
    - [x] Replace the simple PWM control with advanced regulation (e.g., PID control for load compensation).
- [x] **2. Function Outputs**
    - [x] Implement control for at least one function output (e.g., for lighting).
    - [x] Evaluate function commands (F0, F1, etc.) from the `MaerklinMotorola` library.
- [ ] **3. Multi-protocol Capability (RCN-200 Compliance)**
    - [ ] **Protocol Detection:** Implement a robust state machine to detect DCC and MM protocols at runtime as per RCN-200 section 4.1.
    - [ ] **Startup Protocol Selection:** Implement the startup logic defined in RCN-200 section 4.2.
        - [ ] If only one protocol is enabled (via CV 12), use it immediately.
        - [ ] If a preferred protocol is configured, select it upon detection.
        - [ ] If no preference is set, select the first protocol that addresses the decoder.
    - [ ] **Runtime Protocol Switching:** Implement the logic for switching between protocols during operation, as specified in RCN-200 section 4.3 (based on a configurable timeout).
    - [ ] **Analog Mode Handling:** Implement the rules for entering and exiting analog DC and AC modes as defined in RCN-200 section 5.
        - [ ] Enable/disable analog mode via CV 29, Bit 2.
        - [ ] Detect analog signals only when no supported digital protocol is found.
        - [ ] Activate functions in analog mode based on CV 13 and CV 14.
    - [ ] **Smooth Transitions:** Ensure that speed, direction, and function states are maintained or gracefully adjusted when transitioning between any operating modes (digital-digital, digital-analog) as required by RCN-200 section 6.
    - [ ] **CV Implementation:** Add CVs 11, 12, 13, and 14 to `CVManager` and `cv_definitions.h` to support the new functionality.
- [x] **4. Configurability**
    - [x] Implement a way to program the decoder's MM address.
    - [x] Add configurable parameters such as acceleration and braking delay (ABV) via CVs.

## Phase 3: Implementation of Light and Aux Functions

- [x] **1. Core Infrastructure**
    - [x] Implement the core classes for physical outputs and logical functions (`PhysicalOutputManager`, `LogicalFunction`, `FunctionManager`).
    - [x] Implement a CV-based loading mechanism (`CVLoader`, `CVManager`).
- [x] **2. Advanced Effects & Mapping**
    - [x] Implement the complete, multi-level table logic for function mapping (CV-based, `CVLoader`).
    - [x] Implement all light effects provided in the concept (`LIGHT_AND_AUX_CONCEPT.MD`) (`Steady`, `Dimming`, `Flicker`, `Strobe`, `MarsLight`, `SoftStartStop`).
    - [x] Develop a comprehensive CV table for configuration (see `CV_MAPPING.md`).
- [x] **3. Auxiliary Functions**
    - [x] Implement the `SERVO_CONTROL` effect.
    - [x] Implement the `SMOKE_GENERATOR` effect.
- [ ] **4. Tools & Refinement**
    - [ ] Develop a PC-based configuration tool.
    - [ ] Create comprehensive user documentation.

## Phase 4: RCN Standards Compliance

- [x] **1. Standard CV Implementation**
    - [x] Refactor all CV definitions into a central `cv_definitions.h`.
    - [x] Implement all mandatory and applicable recommended CVs from RCN-225.
- [ ] **2. Advanced Function Mapping (RCN-227)**
    - [ ] Implement the RCN-227 function mapping as a new, selectable mode (via CV 96).
    - [ ] The existing proprietary mapping system will be retained as a selectable alternative.
- [x] **3. Documentation Overhaul**
    - [x] Rewrite `CV_MAPPING.md` to reflect the new standardized structure.
    - [x] Translate all project documentation to English.

## Phase 5: Sound System Implementation

- [ ] **1. Core Audio Infrastructure**
    - [ ] Implement the basic hardware drivers (I2S, DMA) and a WAV reader.
- [ ] **2. Polyphony & Prime Mover**
    - [ ] Implement a software mixer for the simultaneous playback of multiple sounds.
    - [ ] Implement a dynamic `PRIME_MOVER` sound.
- [ ] **3. Advanced Mapping & Special Locomotives**
    - [ ] Implement the complete table logic for flexible sound mapping.
    - [ ] Implement the `PRIME_MOVER` logic for steam and electric locomotives.
- [ ] **4. Tools & "Sound Font" Management**
    - [ ] Develop a PC-based tool for creating and managing "sound fonts."

## Phase 6: Hardware Development

- [ ] **1. Requirement Analysis & Design**
    - [ ] Define hardware requirements (size, power, connections).
    - [ ] Select core components (H-bridge, voltage regulator, etc.).
- [ ] **2. Schematic & Layout**
    - [ ] Create the schematic in an EDA tool (e.g., KiCad).
    - [ ] Design the board layout, considering installation dimensions.
- [ ] **3. Prototyping & Testing**
    - [ ] Order prototype boards.
    - [ ] Assemble and commission the first prototypes.
    - [ ] Conduct electrical tests and validate functionality.
- [ ] **4. Documentation**
    - [ ] Create assembly diagrams and a Bill of Materials (BOM).
    - [ ] Publish the hardware design files in the `hardware/` directory.

## Future Ideas

- [ ] RailCom Support
- [ ] LCC/Layout Command Control Integration
- [ ] Support for additional motor types
- [ ] Wireless control (e.g., Wi-Fi, Bluetooth)
