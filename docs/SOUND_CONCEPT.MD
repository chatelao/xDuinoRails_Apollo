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
- **Properties**:
  - **Sample Rate**: 22.05 kHz or 44.1 kHz.
  - **Bit Depth**: 16-bit.
  - **Channels**: Stereo output, allowing for two independent speakers to create more immersive soundscapes.

### 2.2. Sound Slots & Polyphony

A **Sound Slot** is a virtual audio channel capable of playing one sound file at a time. The total number of available slots defines the decoder's **polyphony** (how many sounds can be played simultaneously).

- **Proposal**: **16 simultaneous sound slots** to match the capabilities of high-end decoders and allow for a complex and richly layered soundscape.

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
  - **Implementation**: A "notching" system using multiple sound files for idle and several levels of running speed. The system must be **load-dependent**, meaning it transitions between notches not just based on speed, but also on the current draw of the motor. A locomotive pulling a heavy train up a grade will "notch up" sooner and stay in higher notches longer than one running light on a level track.
  - **Files Needed**: `idle.wav`, `notch_up_1.wav`, `notch_1_coast.wav`, `notch_1_load.wav`, `notch_down_1.wav`, etc.
- **Steam Engine**:
  - **Implementation**: A synchronized "chuff" sound. The system will support multiple synchronization methods:
    - *Sensor-Based (Primary)*: An external sensor (e.g., Hall effect, optical sensor on a wheel, or a physical cam) provides the most precise synchronization. This should be the recommended method for highest realism.
    - *BEMF-Based (Advanced Alternative)*: The motor's Back-EMF is analyzed to detect "cogging" and derive the wheel position, allowing for synchronization without an external sensor. This provides high realism without requiring extra hardware installation.
    - *Time-Based (Fallback)*: Chuff rate is proportional to the speed step. This is the simplest method and serves as a fallback.
  - **Files Needed**: Multiple `chuff_coast_x.wav` and `chuff_load_x.wav` files for variation under different load conditions, `steam_hiss.wav`, `rod_clank.wav`.
- **Electric Locomotive**:
  - **Implementation**: A primary motor whine that changes in pitch and volume with speed. Additional sounds for pantographs, converters, and blowers.
  - **Files Needed**: `motor_whine_loop.wav` (pitch-shifted in real-time), `pantograph_up.wav`, `pantograph_down.wav`, `blower_loop.wav`.

### 3.2. Triggered Sound Effects

These are typically short, distinct sounds triggered by function keys or specific events.

- **Type**: `ONE_SHOT` (plays once) or `CONTINUOUS_LOOP` (plays while trigger is active).
- **Examples**:
  - **Horn/Whistle**: `ONE_SHOT`. Can have different files for short/long blasts, and the mapping system should support "playable" horns where different function keys trigger different parts of a horn sequence.
  - **Bell**: `CONTINUOUS_LOOP`.
  - **Coupler Clank/Release**: `ONE_SHOT`. Can be automatically triggered by a "coupling" function, often linked to the physical movement of a digital coupler ("coupler waltz").

### 3.3. Prototypical Braking Control

To simulate prototypical operation, the decoder will implement a user-controllable, dual-mode braking system. This feature is designed to be used with high-momentum CV settings, where the operator drives the train using the throttle and applies the brakes to slow down, rather than simply reducing the throttle. The user can switch between two distinct braking modes using a function key.

- **Independent (Locomotive) Brake**:
  - **Function**: Applies the brakes on the locomotive(s) only. This mode uses a higher deceleration rate, suitable for light engine moves or for managing slack in a train.
  - **Control**: Activated by a dedicated function key (e.g., F11) when the "Brake Select" function (e.g., F12) is OFF.
  - **Sounds**: A loud `brake_squeal.wav` is played while the brake is active, and an air hiss "bail-off" sound (`bail_off.wav`) is played upon release.

- **Train (Automatic) Brake**:
  - **Function**: Simulates applying the brakes to the entire train. This mode uses a more gradual, configurable deceleration rate.
  - **Control**: Activated by the same dedicated function key (e.g., F11) when the "Brake Select" function (e.g., F12) is ON. Activating the Brake Select function will also trigger a `train_line_charge.wav` sound to simulate preparing the train's air brake system.
  - **Sounds**: Plays a `brake_apply.wav` hiss at the start, followed by a `brake_squeal.wav` (which can be a different, less aggressive sample than the independent brake). A `brake_release.wav` hiss is played upon release.

- **Dynamic Brakes**:
  - **Function**: This is a separate braking system, typically used on diesel and electric locomotives for maintaining speed on downgrades.
  - **Control**: Triggered by a dedicated function key.
  - **Sounds**: A `dynamic_brake_whine.wav` loop is played. The prime mover sound will change to a specific notch (e.g., idle or a mid-range notch, depending on the prototype) while the dynamic brake is active.

- **Parameters**: `volume`, `hold_to_loop` (for horns), `fade_in_time`, `fade_out_time`.

### 3.4. Ambient & Automated Sounds

These sounds create a living soundscape and are often not directly controlled by the user.

- **Type**: `RANDOM_AMBIENT`
- **Implementation**: The sound is played at random intervals when the locomotive is in a specific state (e.g., stopped or moving slowly).
- **Examples**: Air compressor pump, generator whine, cab radio chatter, track noise, injector sounds, "Fireman Ed" / crew chatter (e.g., "shoveling coal"), power reverser sounds.
- **Parameters**: `min_delay`, `max_delay`, `trigger_state` (e.g., `stopped`, `moving`).

## 4. Audio Engine & Hardware

### 4.1. Sound Storage

High-quality sound requires significant storage.
- **Proposal**: Use an onboard SPI flash chip of **128 Mbit (16 MB) or greater**. This is the current standard for high-end decoders and provides ample space for uncompressed 16-bit WAV files for a full sound profile.

### 4.2. High-Speed Sound Loading

To facilitate the rapid updating of sound collections, a dedicated high-speed interface is required. Relying on the DCC track signal is too slow for 16 MB of data.
- **Proposal**: Implement a sound-loading protocol over a dedicated hardware interface. Options include:
  - **SUSI Interface**: Use the SUSI clock/data pins with a proprietary high-speed protocol, as is common with commercial decoders.
  - **USB-to-Serial**: Integrate a USB-to-serial converter (like a CH340) on the decoder board for a direct connection to a PC. This is a more user-friendly option.

### 4.3. File System

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
    SoundSlot slots[16];
    void mixAudioToI2S(); // Called by DMA interrupt
};
```


## 5. VSD Integration: Concept of Operations

To provide maximum flexibility and leverage a well-established standard, the decoder will natively support the **JMRI Virtual Sound Decoder (VSD)** file format. This section details the concept of operations for handling and parsing these files.

### 5.1. VSD File Handling

A VSD file is a `.vsd` zip archive containing `.wav` files and a `config.xml` manifest. The decoder firmware will handle this archive in memory.

1.  **Loading**: Upon startup or when commanded, the decoder will load the entire `.vsd` file from its flash storage into a RAM buffer.
2.  **Decompression**: The firmware will use the lightweight **`miniz`** library to access the contents of the zip archive directly from the RAM buffer.
3.  **XML Extraction**: The `miniz` library will be used to locate and decompress the `config.xml` file into a separate text buffer. The `.wav` files will be left compressed in the archive for now.
4.  **XML Parsing**: The **`expat`** XML parser library will be used to parse the `config.xml` from its text buffer. `expat` is a stream-oriented (SAX) parser, which is highly efficient in memory usage as it does not need to build a complete Document Object Model (DOM) tree.

### 5.2. Core `config.xml` Concepts

The VSD `config.xml` defines all sound logic. The `expat`-based parser will read this file and translate its contents into the decoder's internal sound event and action lists.

- **`<sound>` Tags**: Each `<sound>` tag will be parsed to create an internal "Logical Sound" object. The parser will extract the sound `type` (e.g., `steam1`, `horn`), the associated `.wav` file name, and parameters like gain (`<gain>`).
- **`<sound-event>` Tags**: The parser will iterate through the `<trigger>` tags within each `<sound-event>` block.
- **`<trigger>` and `<action>` Tags**: For each `<trigger>`, the parser will create a mapping rule that links the trigger condition (e.g., "F2 ON") to the specified `<action>` (e.g., "PLAY") on the target `<sound>`.

This parsing process effectively translates the declarative XML structure into an executable logic map that the decoder's sound engine can run.

### 5.3. Supported Features (Phase 1)

The initial implementation will focus on supporting a core subset of the VSD specification to provide a functional and useful sound experience.

- **Supported Engine Types**:
  - `steam1` (generic steam engine with chuff control)
  - `diesel3` (generic diesel with notching)
- **Supported Sound Types**:
  - `horn`, `bell`, `coupler`
- **Supported Triggers**:
  - `THROTTLE` (for function keys)
  - `SPEED` (for speed-based events)
  - `BRAKE_KEY`
  - `COAST`
- **Supported Actions**:
  - `PLAY`, `LOOP`, `STOP`, `FADE_OUT`

### 5.4. Integration with Decoder CVs

While the VSD file defines the core logic, users will still need to configure certain parameters. The decoder will expose CVs to override or adjust settings from the VSD file.

- **CV Override**: A CV will be provided to adjust the `<gain>` (volume) of each `<sound>` tag, identified by its order in the `config.xml` file (e.g., Sound 1 Volume, Sound 2 Volume, etc.).
- **Brake & Accel Rates**: CVs will be provided to set the `<brake-time>`, `<accel-rate>`, and `<decel-rate>` values referenced by the VSD logic.
- **JMRI Integration**: The JMRI DecoderPro definition file will be updated to give user-friendly names to these CVs, making it clear which CV adjusts the volume for the "Horn", "Bell", etc., based on the loaded VSD profile.

## 6. Protocol-Specific Considerations

### 6.1. DCC

DCC is the ideal protocol for a feature-rich sound decoder.
- **Functions**: F0-F28 provide ample triggers for the events defined in the VSD's `config.xml`.
- **Configuration**: While the VSD file defines the sound logic, key parameters such as individual sound volumes (`<gain>`), acceleration rates, and braking times can be exposed and configurable via CVs.

### 6.2. Märklin-Motorola (MM)

The limited function keys (F0-F4) in MM present a challenge.
- **Strategy**: The mapping system allows for intelligent use of the limited keys.
  - `F0`: Controls direction-dependent prime mover sounds (on/off).
  - `F1`: Could be a momentary horn.
  - `F2`: Could be a latching bell.
  - The system can be configured so that a short press of F1 is a short horn blast, while a long press is a continuous blast.
  - Automated sounds (brakes, compressors) are triggered by locomotive state, not function keys, so they will work fully under MM.

## 7. Configuration & Tooling

A powerful sound system is only useful if it can be easily configured by the end-user. The configuration will be handled through a combination of CVs and external tools.

### 7.1. CV Layout

The configuration variables (CVs) will be organized in a logical manner, following established RCN standards where applicable. Key configuration areas will include:

- **Master Volumes**: Individual CVs to control the master volume of the prime mover, horn, bell, and other effects categories.
- **Individual Sound Volumes**: Indexed CVs will be used to set the volume for each individual `Logical Sound` defined in the sound profile.
- **Prime Mover Settings**: CVs for configuring the chuff rate for steam engines, diesel notch points, and load sensitivity.
- **Function Mapping**: The flexible multi-level logic table will be configurable via a large block of indexed CVs, as outlined in the RCN-227 standard.

While direct CV programming will be possible, it is not the recommended method for initial setup due to the complexity of the mapping system.

### 7.2. JMRI DecoderPro Integration

To provide a user-friendly configuration experience, creating a comprehensive decoder definition file for JMRI DecoderPro is a primary goal.

- **Implementation**: An XML file will be created that defines all supported CVs, their valid ranges, and their meanings.
- **Features**:
  - **Intuitive Panes**: The definition will use DecoderPro's pane-based layout to group related settings (e.g., "Sound Volumes," "Function Mapping," "Motor Control").
  - **User-Friendly Labels**: All CVs and options will have clear, descriptive English labels.
  - **Dynamic Sound Profile Support**: The DecoderPro definition will be designed to work in conjunction with the loaded `Sound Collection`. It will allow users to fine-tune the volumes and basic parameters of the sounds defined in the collection, but the core sound-to-file mapping will be managed by the collection itself.

The goal is to allow users to perform all common configuration tasks—such as adjusting volumes, remapping functions, and setting motor parameters—through the familiar DecoderPro interface, without needing to manually edit individual CVs.

## 8. Proposed Implementation Roadmap

### Phase 1: Core Audio Infrastructure

- **Goal**: Get a single sound playing from storage.
- **Tasks**:
  - [ ] 1. Finalize hardware choice (microcontroller, flash/SD, I2S DAC/Amp).
  - [ ] 2. Implement I2S and DMA drivers.
  - [ ] 3. Implement the file system and a WAV file reader.
  - [ ] 4. Create a single Sound Slot that can play a `ONE_SHOT` sound triggered by a function key.
- **Outcome**: A "beep-on-F1" proof of concept.

### Phase 2: Polyphony & Prime Mover

- **Goal**: Implement multi-sound playback and dynamic engine sounds.
- **Tasks**:
  - [ ] 1. Implement the software mixer and multiple Sound Slots for polyphony.
  - [ ] 2. Implement the `PRIME_MOVER` sound type for a diesel, with basic notching logic tied to speed step.
  - [ ] 3. Implement basic automated sounds like brake squeal.
- **Outcome**: A decoder with basic, but working, diesel engine sounds and effects.

### Phase 3: Advanced Mapping & Steam/Electric

- **Goal**: Implement the full, flexible sound control system and other engine types.
- **Tasks**:
  - [ ] 1. Implement the full multi-level table logic for sound mapping.
  - [ ] 2. Implement the steam `PRIME_MOVER` with BEMF-based chuff synchronization.
  - [ ] 3. Implement the electric `PRIME_MOVER` with pitch-shifting.
  - [ ] 4. Implement `RANDOM_AMBIENT` sounds.
- **Outcome**: A full-featured sound decoder with highly configurable and realistic sound.

### Phase 4: VSD Parser Implementation

- **Goal**: Read and execute a VSD file from the decoder's storage.
- **Tasks**:
  - [ ] 1. Select and integrate a lightweight embedded XML parsing library.
  - [ ] 2. Implement the VSD parser logic to read `config.xml` and configure the sound engine.
  - [ ] 3. Add a mechanism to upload `.vsd` files to the decoder's flash storage via the high-speed interface.
- **Outcome**: The decoder can be fully configured by loading a standard JMRI VSD file.
