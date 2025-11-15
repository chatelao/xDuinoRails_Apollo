# Sound System Implementation Roadmap

This document breaks down the implementation of the sound system into manageable phases and tasks, based on the full concept outlined in `SOUND_CONCEPT.MD`.

## Phase 1: Core Audio Infrastructure

**Goal**: Get a single sound playing from storage.

- [ ] **1.1. Hardware Finalization**: Finalize the choice of microcontroller, flash/SD storage, and I2S DAC/Amplifier.
- [ ] **1.2. I2S & DMA Drivers**: Implement the low-level I2S and DMA drivers for audio output.
- [ ] **1.3. File System & WAV Reader**: Implement the file system for the chosen storage medium and a robust WAV file reader.
- [ ] **1.4. Proof of Concept**: Create a single Sound Slot that can play a `ONE_SHOT` sound (e.g., a simple beep) triggered by a DCC function key.

## Phase 2: Polyphony & Prime Mover

**Goal**: Implement multi-sound playback and dynamic engine sounds.

- [ ] **2.1. Software Mixer & Polyphony**: Implement the software mixer and expand to multiple Sound Slots (e.g., 16) to enable polyphonic sound.
- [ ] **2.2. Diesel Prime Mover**: Implement the `PRIME_MOVER` sound type for a diesel locomotive, with basic notching logic tied to the speed step.
- [ ] **2.3. Automated Brake Sounds**: Implement basic automated sounds, such as a brake squeal that triggers on deceleration.

## Phase 3: Advanced Mapping & Steam/Electric Sounds

**Goal**: Implement the full, flexible sound control system and other engine types.

- [ ] **3.1. Advanced Mapping Logic**: Implement the full multi-level table logic for sound mapping, allowing complex triggers and conditions.
- [ ] **3.2. Steam Prime Mover**: Implement the steam `PRIME_MOVER`, including the BEMF-based chuff synchronization logic.
- [ ] **3.3. Electric Prime Mover**: Implement the electric `PRIME_MOVER`, including real-time pitch-shifting for the motor whine.
- [ ] **3.4. Ambient Sound System**: Implement the `RANDOM_AMBIENT` sound type for automated, randomly triggered background sounds.

## Phase 4: VSD Parser Implementation

**Goal**: Read and execute a standard JMRI VSD file from the decoder's storage.

- [ ] **4.1. XML Parser Integration**: Select and integrate a lightweight, embedded XML parsing library (e.g., `expat`).
- [ ] **4.2. VSD Parser Logic**: Implement the parser logic to read `config.xml` from a `.vsd` archive and dynamically configure the sound engine based on its contents.
- [ ] **4.3. High-Speed File Upload**: Implement a mechanism to upload `.vsd` files to the decoder's flash storage via a high-speed interface (e.g., SUSI or USB-Serial).
