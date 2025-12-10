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
- [ ] **3. Multi-protocol Capability (Simultaneous Operation)**
    - [ ] Extend the decoder to automatically distinguish between DCC and MM signals at runtime.
    - [ ] Create a common interface for motor and function control that can be used by both protocol implementations.
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

- [ ] **1. VSD File Handling & Basic Playback**
    - [ ] Integrate libraries for filesystem (`LittleFS`), ZIP decompression (`miniz`), and XML parsing (`expat`).
    - [ ] Implement a VSD file handler to load and extract sound and configuration data.
    - [ ] Implement a basic `ONE_SHOT` sound trigger based on a VSD `config.xml`.
- [ ] **2. Polyphony and Advanced Sound Types**
    - [ ] Implement a software mixer for polyphonic playback of up to 16 channels.
    - [ ] Implement `CONTINUOUS_LOOP` and `RANDOM_AMBIENT` sound types.
    - [ ] Implement a dynamic, load-dependent `PRIME_MOVER` sound for diesel locomotives.
- [ ] **3. Advanced VSD Integration & Special Locomotives**
    - [ ] Implement the full VSD specification, including all trigger and action types.
    - [ ] Implement `PRIME_MOVER` logic for steam and electric locomotives.
    - [ ] Implement CV overrides for VSD parameters and create a JMRI DecoderPro definition file.

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

## Phase 7: Common Functional Enhancements

- [ ] **1. Braking Systems**
    - [ ] Implement ABC Braking (Automatic Braking Control).
    - [ ] Implement DC Braking support.
    - [ ] Implement Constant Braking Distance logic.
- [ ] **2. Operational Features**
    - [ ] Implement Shunting Mode (half speed, reduced momentum).
    - [ ] Implement Momentum Disable (ABV Off) via function key.
    - [ ] Implement Advanced Consisting (CV 19).
    - [ ] Implement Speed Tables (3-point and 28-point).
- [ ] **3. Advanced Effects**
    - [ ] Implement Coupler Waltz (Kupplungswalzer).
- [ ] **4. Hardware Support**
    - [ ] PowerPack / Keep-alive support.

## Phase 8: High-End / Premium Features (Research Findings)

- [ ] **1. Advanced Motor Control**
    - [ ] Implement "Autotune" (Automatic Motor Calibration).
    - [ ] Implement Variable PWM Frequency (10kHz - 50kHz).
- [ ] **2. Simulation & Physics**
    - [ ] Implement Consumables Simulation (Fuel, Water, Sand) with load dependence.
    - [ ] Implement "Coasting Mode" logic (performance reduction when empty).
    - [ ] Implement Heavy Load Simulation.
- [ ] **3. Dynamic Sound Enhancements**
    - [ ] Implement Dynamic Digital Exhaust (DDE) / Load-dependent sound intensity.
    - [ ] Implement Grade Crossing Logic (Auto-sequenced horn/bell).
    - [ ] Implement Audio Equalizer (7-band) and Reverb effects.
- [ ] **4. Safety & Feedback**
    - [ ] Implement Dead Man's Button (Sifa) logic.
    - [ ] Implement Software Overload Protection (current monitoring).
    - [ ] Support for External Sensors (Wheel Cam/Hall Sensor) for synchronized sound.

## Phase 9: Scripting & Automation (Lua)

- [ ] **1. Interpreter Integration**
    - [ ] Select and integrate a lightweight Lua interpreter (e.g., standard Lua 5.4, eLua) suitable for the RP2040.
    - [ ] Configure the build system (PlatformIO) to compile and link the Lua engine.
- [ ] **2. Core Bindings (CV Access)**
    - [ ] Implement C++/Lua bindings to expose the `CVManager` interface.
    - [ ] Provide Lua functions for reading and writing CVs (e.g., `cv.read(address)`, `cv.write(address, value)`).
- [ ] **3. Script Management**
    - [ ] Implement a mechanism to load Lua scripts from the filesystem (LittleFS).
    - [ ] Define the script lifecycle (e.g., `init.lua` on startup, event-driven callbacks).

## Future Ideas

- [ ] RailCom Support
- [ ] LCC/Layout Command Control Integration
- [ ] Support for additional motor types
- [ ] Wireless control (e.g., Wi-Fi, Bluetooth)
