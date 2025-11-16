# Sound System Implementation Roadmap

This document provides a detailed, task-level roadmap for the implementation of the VSD-based sound system.

## 1. VSD File Handling & Basic Playback

- [ ] **1.1. Library Integration:**
    - [ ] Add the `LittleFS` library to `platformio.ini` for filesystem management.
    - [ ] Add the `miniz` library for in-memory ZIP decompression.
    - [ ] Add the `expat` library for XML parsing.
    - [ ] Verify that the firmware compiles successfully with the new libraries.

- [ ] **1.2. Filesystem & VSD Loader:**
    - [ ] Implement a `FileSystem` wrapper class to abstract filesystem operations.
    - [ ] Implement a `VSDReader` class responsible for loading a `.vsd` file.
    - [ ] Implement a method in `VSDReader` to decompress the archive using `miniz`.
    - [ ] Implement a method to extract raw file data (e.g., `config.xml`, `.wav` files) by name from the in-memory archive.

- [ ] **1.3. Basic Audio Playback:**
    - [ ] Create a `WAVPlayer` class.
    - [ ] Implement a method in `WAVPlayer` to parse a `.wav` header.
    - [ ] Implement a method to stream audio samples from the `WAVPlayer` to the existing I2S hardware driver.
    - [ ] Create a proof-of-concept example that plays a bundled `.wav` file on startup.

- [ ] **1.4. Basic VSD `config.xml` Parsing:**
    - [ ] Create a `VSDConfigParser` class that uses the `expat` library.
    - [ ] Implement parsing for a `<sound>` tag of type `ONE_SHOT`.
    - [ ] Implement parsing for a `<trigger>` tag linked to a DCC function key.
    - [ ] Store the parsed sound and trigger information in simple data structures.

- [ ] **1.5. System Integration:**
    - [ ] Instantiate the `VSDReader` and `VSDConfigParser` in the main firmware.
    - [ ] Load and parse a sample `sound.vsd` file from the filesystem on startup.
    - [ ] In the main loop, check for the parsed DCC function key trigger.
    - [ ] When triggered, use the `WAVPlayer` to play the corresponding `ONE_SHOT` sound extracted from the VSD archive.

## 2. Polyphony and Advanced Sound Types

- [ ] **2.1. Software Mixer:**
    - [ ] Create a `SoftwareMixer` class.
    - [ ] Implement logic to manage 16 audio channels.
    - [ ] Implement a mixing algorithm to combine all active channels into a single stereo output stream.
    - [ ] Refactor the `WAVPlayer` to become a "sound source" that feeds its audio data into a mixer channel instead of directly to the I2S driver.

- [ ] **2.2. Advanced Sound Types:**
    - [ ] Implement the `CONTINUOUS_LOOP` sound type.
    - [ ] Implement the `RANDOM_AMBIENT` sound type with configurable interval and probability.
    - [ ] Update the `VSDConfigParser` to handle the new sound type attributes.

- [ ] **2.3. Prime Mover (Diesel):**
    - [ ] Create a `PrimeMover` sound class.
    - [ ] Implement logic to manage multiple sound samples for different "notches."
    - [ ] Implement cross-fading between notch samples.
    - [ ] Implement a state machine to control the prime mover based on decoder speed and simulated load.
    - [ ] Update the `VSDConfigParser` to handle the `diesel3` engine type.

## 3. Advanced VSD Integration & Special Locomotives

- [ ] **3.1. Full VSD Specification:**
    - [ ] Implement all VSD trigger types (`THROTTLE`, `SPEED`, `BRAKE_KEY`, `COAST`, etc.).
    - [ ] Implement all VSD action types (`PLAY`, `LOOP`, `STOP`, `FADE_OUT`, etc.).
    - [ ] Implement a robust `TriggerManager` to evaluate the complete ruleset from the `config.xml` on each main loop iteration.

- [ ] **3.2. Prime Mover (Steam & Electric):**
    - [ ] Implement the `steam1` prime mover logic with synchronized chuff.
    - [ ] Implement the `electric1` prime mover logic with real-time pitch shifting.
    - [ ] Update the `VSDConfigParser` to handle the new engine types.

- [ ] **3.3. CV Integration & Tooling:**
    - [ ] Implement CVs to override VSD parameters (e.g., master volume, individual sound volumes).
    - [ ] Create a comprehensive JMRI DecoderPro definition file (`.xml`) for all sound-related CVs.
    - [ ] Document the sound CVs in `CV_MAPPING.md`.
