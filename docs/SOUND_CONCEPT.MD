# Concept: State-of-the-Art Sound System

## 1. Introduction

This document outlines a comprehensive concept for a state-of-the-art, polyphonic sound system for a multi-protocol (DCC and Märklin-Motorola) model railway decoder. The goal is to create a flexible, high-fidelity, and user-configurable framework that rivals the features of high-end commercial sound decoders, leveraging the power of modern microcontrollers.

This concept covers:
- Realistic, dynamic prime mover (engine) sounds.
- A wide range of triggered and automated sound effects.
- A powerful mapping system to link command station controls and locomotive state to sound events.
- An architecture that supports high-quality, uncompressed audio playback.

## 2. Core Concepts

A flexible sound system requires separating the audio playback hardware from the logical sound events. This allows complex soundscapes to be created and customized without altering the firmware.

### 2.1. Audio Output

The **Audio Output** is the physical hardware responsible for generating the analog audio signal. For high-fidelity sound, this concept is based on an I2S (Inter-IC Sound) interface.

- **Type**: `I2S_DAC_AMPLIFIER`. This assumes the use of an external I2S DAC and a Class-D amplifier for driving the speaker, providing superior audio quality compared to simple PWM audio.
- **Properties**: Sample rate (e.g., 22.05 kHz or 44.1 kHz), bit depth (e.g., 16-bit).

### 2.2. Sound Slots & Polyphony

A **Sound Slot** is a virtual audio channel capable of playing one sound file at a time. The total number of available slots defines the decoder's **polyphony** (how many sounds can be played simultaneously).

- **Proposal**: A minimum of 8-12 simultaneous sound slots to allow for a complex and layered soundscape (e.g., engine, horn, bell, and brake squeal all at once).

### 2.3. Logical Sounds

A **Logical Sound** represents a specific sound feature of the locomotive, such as "Diesel Engine," "Primary Horn," "Brake Squeal," or "Air Compressor." It is an abstract representation, which is then mapped to a specific sound file and triggered by events.

Each Logical Sound has:
- **Sound Type**: Defines its behavior (e.g., `PRIME_MOVER`, `CONTINUOUS_LOOP`, `ONE_SHOT`, `RANDOM_AMBIENT`).
- **Parameters**: A set of values for configuration (e.g., `volume`, `pitch_shift`, `trigger_thresholds`).
- **Sound File**: A reference to a specific `.wav` file on the storage medium.

### 2.4. Triggers & Function Mapping

**Triggers** are events that activate, deactivate, or modify Logical Sounds. The **Function Mapping** system links these triggers to the sounds, forming the core of the user's configuration.

Triggers can be:
- **Function Keys**: Pressing a function key (e.g., F1, F2).
- **Decoder State**: `DIRECTION_FORWARD`, `IS_MOVING`, `ACCELERATING`, `DECELERATING`.
- **Speed Step**: The current speed step of the locomotive.
- **Analog Input**: A sensor value, such as a load measurement from the motor control (for realistic engine load effects).

## 3. Sound Features & Effects

This section details the proposed sound features, which would be implemented as "Sound Types" for Logical Sounds.

### 3.1. Prime Mover Sounds

This is the most critical and complex part of the sound system. The goal is a dynamic, responsive, and believable engine sound.

- **Type**: `PRIME_MOVER`
- **Diesel Engine**:
  - **Implementation**: A "notching" system using multiple sound files for idle, and several levels of acceleration, steady speed, and deceleration. The system transitions smoothly between these sound files based on speed step and acceleration rate.
  - **Files Needed**: `idle.wav`, `notch_up_1.wav`, `notch_1.wav`, `notch_down_1.wav`, `notch_up_2.wav`, etc.
- **Steam Engine**:
  - **Implementation**: A synchronized "chuff" sound.
    - *Variant A (Simple Time-Based)*: Chuff rate is proportional to the speed step.
    - *Variant B (Sensor-Based)*: An external sensor (e.g., Hall effect on a wheel) provides perfect synchronization.
    - *Variant C (BEMF-Based)*: The motor's Back-EMF is analyzed to detect "cogging" and derive the wheel position, allowing for synchronization without an external sensor.
  - **Proposal**: Implement Variant C as it provides high realism without requiring extra hardware installation.
  - **Files Needed**: `chuff_1.wav`, `chuff_2.wav` (for variation), `steam_hiss.wav`, `rod_clank.wav`.
- **Electric Locomotive**:
  - **Implementation**: A primary motor whine that changes in pitch and volume with speed. Additional sounds for pantographs, converters, and blowers.
  - **Files Needed**: `motor_whine_loop.wav` (pitch-shifted in real-time), `pantograph_up.wav`, `pantograph_down.wav`, `blower_loop.wav`.

### 3.2. Triggered Sound Effects

These are typically short, distinct sounds triggered by function keys or specific events.

- **Type**: `ONE_SHOT` (plays once) or `CONTINUOUS_LOOP` (plays while trigger is active).
- **Examples**:
  - **Horn/Whistle**: `ONE_SHOT`. Can have different files for short/long blasts.
  - **Bell**: `CONTINUOUS_LOOP`.
  - **Coupler Clank**: `ONE_SHOT`. Can be automatically triggered by a "coupling" function.
  - **Brake Squeal**: `ONE_SHOT` or `CONTINUOUS_LOOP`. Automatically triggered by deceleration.
- **Parameters**: `volume`, `hold_to_loop` (for horns), `fade_in_time`, `fade_out_time`.

### 3.3. Ambient & Automated Sounds

These sounds create a living soundscape and are often not directly controlled by the user.

- **Type**: `RANDOM_AMBIENT`
- **Implementation**: The sound is played at random intervals when the locomotive is in a specific state (e.g., stopped or moving slowly).
- **Examples**: Air compressor pump, generator whine, cab radio chatter, track noise.
- **Parameters**: `min_delay`, `max_delay`, `trigger_state` (e.g., `stopped`, `moving`).

## 4. Audio Engine & Hardware

### 4.1. Sound Storage

High-quality sound requires significant storage.
- **Proposal**: Use a microSD card or an onboard SPI flash chip (e.g., 16-32 MB) to store sound files. This provides ample space for uncompressed 16-bit WAV files.

### 4.2. File System

A simple file system is required to access the sound files on the storage medium.
- **Proposal**: Use a lightweight library like `FatFS` for microSD cards or `LittleFS` for SPI flash.

### 4.3. Audio Playback Engine

- **Implementation**: The core of the engine will be an I2S driver fed by a DMA controller. This allows the CPU to offload the work of sending audio data, leaving it free for mixing and game logic. A software mixer will combine the outputs of all active Sound Slots into a single audio stream for the I2S hardware.
- **Features**: Per-slot volume control, real-time pitch shifting (for motor sounds), and smooth cross-fading between sound loops.

```cpp
// Pseudo-code for conceptual illustration
class SoundSlot {
public:
    void play(const char* wav_file, float volume);
    void stop();
    void setVolume(float vol);
    bool isPlaying();
};

class AudioEngine {
    SoundSlot slots[12];
    void mixAudioToI2S(); // Called by DMA interrupt
};
```

## 5. Function Mapping System

The flexible mapping system from the Light & AUX concept will be adapted for sound. This allows the user to define precisely how sounds are triggered.

### 5.1. Multi-level Table Logic

The proposed **Multi-level Table Logic (Variant C)** is perfect for sound. It allows complex combinations of triggers to control sound events.

1.  **Condition Variables**: Define booleans based on decoder state.
    - `C1 = F_STATE(2, ON)` (Horn button)
    - `C2 = SPEED_IS(ZERO)` (Stopped)
    - `C3 = IS_DECELERATING()`
2.  **Logic Table**: Use boolean logic to trigger sounds.
    - `IF (C1 AND C2) THEN Play "Horn_Stopped.wav"`
    - `IF (C1 AND NOT C2) THEN Play "Horn_Moving.wav"`
    - `IF (C3) THEN Play "Brake_Squeal.wav"`

This provides incredible power, allowing for context-sensitive sound playback.

## 6. Protocol-Specific Considerations

### 6.1. DCC

DCC is the ideal protocol for a feature-rich sound decoder.
- **Functions**: F0-F28 provide ample triggers for a wide variety of sounds (horn, bell, compressor, etc.).
- **Configuration**: All sound options, volumes, and the entire function mapping table will be configurable via CVs. A PC-based tool is highly recommended for managing the "sound profile" (the collection of WAV files and CV settings).

### 6.2. Märklin-Motorola (MM)

The limited function keys (F0-F4) in MM present a challenge.
- **Strategy**: The mapping system allows for intelligent use of the limited keys.
  - `F0`: Controls direction-dependent prime mover sounds (on/off).
  - `F1`: Could be a momentary horn.
  - `F2`: Could be a latching bell.
  - The system can be configured so that a short press of F1 is a short horn blast, while a long press is a continuous blast.
  - Automated sounds (brakes, compressors) are triggered by locomotive state, not function keys, so they will work fully under MM.

## 7. Proposed Implementation Roadmap

### Phase 1: Core Audio Infrastructure

- **Goal**: Get a single sound playing from storage.
- **Tasks**:
  1. Finalize hardware choice (microcontroller, flash/SD, I2S DAC/Amp).
  2. Implement I2S and DMA drivers.
  3. Implement the file system and a WAV file reader.
  4. Create a single Sound Slot that can play a `ONE_SHOT` sound triggered by a function key.
- **Outcome**: A "beep-on-F1" proof of concept.

### Phase 2: Polyphony & Prime Mover

- **Goal**: Implement multi-sound playback and dynamic engine sounds.
- **Tasks**:
  1. Implement the software mixer and multiple Sound Slots for polyphony.
  2. Implement the `PRIME_MOVER` sound type for a diesel, with basic notching logic tied to speed step.
  3. Implement basic automated sounds like brake squeal.
- **Outcome**: A decoder with basic, but working, diesel engine sounds and effects.

### Phase 3: Advanced Mapping & Steam/Electric

- **Goal**: Implement the full, flexible sound control system and other engine types.
- **Tasks**:
  1. Implement the full multi-level table logic for sound mapping.
  2. Implement the steam `PRIME_MOVER` with BEMF-based chuff synchronization.
  3. Implement the electric `PRIME_MOVER` with pitch-shifting.
  4. Implement `RANDOM_AMBIENT` sounds.
- **Outcome**: A full-featured sound decoder with highly configurable and realistic sound.

### Phase 4: Tooling & "Sound Font" Management

- **Goal**: Create a user-friendly way to manage sound profiles.
- **Tasks**:
  1. Develop a PC-based tool for creating "sound fonts" (a package containing WAV files and a CV configuration file).
  2. The tool should allow users to easily upload a complete sound profile to the decoder's storage.
  3. Write comprehensive user documentation.
- **Outcome**: A mature, powerful, and user-friendly sound decoder platform.
