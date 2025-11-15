# Concept: State-of-the-Art Sound System

## 1. Introduction

This document outlines a comprehensive concept for a state-of-the-art, polyphonic sound system for a multi-protocol (DCC and Märklin-Motorola) model railway decoder. The goal is to create a flexible, high-fidelity, and user-configurable framework that rivals the features of high-end commercial sound decoders, leveraging the power of modern microcontrollers.

- [ ] Support realistic, dynamic prime mover (engine) sounds.
- [ ] Support a wide range of triggered and automated sound effects.
- [ ] Implement a powerful mapping system to link command station controls and locomotive state to sound events.
- [x] Create an architecture that supports multiple, selectable hardware output options.

- **Implementation Progress:** The core architecture for supporting multiple hardware options has been implemented.

## 2. Core Concepts

A flexible sound system requires separating the audio playback hardware from the logical sound events. This allows complex soundscapes to be created and customized without altering the firmware.

- [x] **2.1. Audio Output:**
    - [x] Implement support for I2S DACs.
    - [x] Support sample rates of 22.05 kHz or 44.1 kHz.
    - [x] Support 16-bit audio.
    - [ ] Support stereo output.
- [x] **2.2. Sound Slots & Polyphony:**
    - [x] Implement 16 simultaneous sound slots.
- [ ] **2.3. Logical Sounds:**
    - [ ] Create a `LogicalSound` class.
    - [ ] Implement sound types: `PRIME_MOVER`, `CONTINUOUS_LOOP`, `ONE_SHOT`, `RANDOM_AMBIENT`.
- [ ] **2.4. Triggers & Function Mapping:**
    - [ ] Implement a trigger system for function keys.
    - [ ] Implement triggers for decoder state changes.
    - [ ] Implement triggers for speed step changes.
    - [ ] Implement triggers for analog input (motor load).

- **Implementation Progress:** The core polyphonic audio engine has been implemented. It features a 16-channel software mixer with volume control. The system uses a hardware abstraction layer, allowing the engine to work with drivers that support raw PCM playback (e.g., I2S), while gracefully degrading for simpler drivers (e.g., DFPlayer Mini).

## 3. Sound Features & Effects

- [ ] **3.1. Prime Mover Sounds:**
    - [ ] Implement a load-dependent diesel "notching" system.
    - [ ] Implement a synchronized steam "chuff" system with sensor, BEMF, and time-based options.
    - [ ] Implement an electric locomotive motor whine with real-time pitch shifting.
- [ ] **3.2. Triggered Sound Effects:**
    - [ ] Implement `ONE_SHOT` and `CONTINUOUS_LOOP` sound types.
    - [ ] Support playable horns.
- [ ] **3.3. Prototypical Braking Control:**
    - [ ] Implement an independent (locomotive) brake.
    - [ ] Implement a train (automatic) brake.
    - [ ] Implement dynamic brakes.
- [ ] **3.4. Ambient & Automated Sounds:**
    - [ ] Implement a `RANDOM_AMBIENT` sound type.

- **Implementation Progress:** Not yet started.

## 4. Audio Engine & Hardware

- [ ] **4.1. Sound Storage:**
    - [ ] Implement support for an onboard SPI flash chip (>= 128 Mbit).
- [ ] **4.2. High-Speed Sound Loading:**
    - [ ] Implement a sound-loading protocol over a SUSI or USB-to-Serial interface.
- [ ] **4.3. File System:**
    - [ ] Integrate a lightweight file system library (`FatFS` or `LittleFS`).
- [x] **4.4. Hardware Drivers:**
    - [x] Implement a high-fidelity I2S driver.
    - [x] Implement a basic DFPlayer Mini driver.
    - [x] Implement a low-cost PWM DAC driver.
    - [x] Implement a raw PCM digital output driver.

- **Implementation Progress:** The core drivers for all four hardware options have been implemented with a proof-of-concept "beep" sound.

## 5. VSD Integration: Concept of Operations

- [ ] **5.1. VSD File Handling:**
    - [ ] Implement loading of `.vsd` files from flash into RAM.
    - [ ] Integrate the `miniz` library for in-memory decompression.
    - [ ] Integrate the `expat` library for XML parsing.
- [ ] **5.2. Core `config.xml` Parsing:**
    - [ ] Parse `<sound>` tags into `LogicalSound` objects.
    - [ ] Parse `<sound-event>`, `<trigger>`, and `<action>` tags into a mapping ruleset.
- [ ] **5.3. Supported Features (Phase 1):**
    - [ ] Support `steam1` and `diesel3` engine types.
    - [ ] Support `horn`, `bell`, and `coupler` sound types.
    - [ ] Support `THROTTLE`, `SPEED`, `BRAKE_KEY`, and `COAST` triggers.
    - [ ] Support `PLAY`, `LOOP`, `STOP`, and `FADE_OUT` actions.
- [ ] **5.4. Integration with Decoder CVs:**
    - [ ] Implement CV overrides for VSD parameters (e.g., volume).
    - [ ] Create a JMRI DecoderPro definition file for VSD-related CVs.

- **Implementation Progress:** Not yet started.

## 6. Protocol-Specific Considerations

- [ ] Implement DCC function key mapping (F0-F28).
- [ ] Implement Märklin-Motorola function key mapping (F0-F4).

- **Implementation Progress:** Not yet started.

## 7. Configuration & Tooling

- [ ] **7.1. CV Layout:**
    - [ ] Implement master and individual sound volume CVs.
    - [ ] Implement prime mover setting CVs.
    - [ ] Implement the RCN-227 function mapping CVs.
- [ ] **7.2. JMRI DecoderPro Integration:**
    - [ ] Create a comprehensive decoder definition file for JMRI DecoderPro.

- **Implementation Progress:** Not yet started.
