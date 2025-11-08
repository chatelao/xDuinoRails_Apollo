# Concept: State-of-the-Art Light & AUX Management

## 1. Introduction

This document outlines a comprehensive concept for a state-of-the-art lighting and auxiliary function management system for a multi-protocol (DCC and Märklin-Motorola) model railway decoder. The goal is to create a flexible, powerful, and user-friendly framework that rivals the features of high-end commercial decoders while remaining open and accessible.

This concept covers:
- Advanced lighting effects.
- Control of auxiliary hardware (servos, smoke generators).
- A powerful function mapping system to link command station controls to decoder actions.
- Considerations for both DCC and Märklin-Motorola protocols.

## 2. Core Concepts

To build a flexible system, it's crucial to separate the physical hardware from the logical functions. This decoupling allows for complex behaviors to be configured without changing the underlying code.

### 2.1. Physical Outputs

A **Physical Output** is a pin on the microcontroller that can be controlled to provide power to a device. Each output has defined physical properties.

- **Type**: Defines the electrical nature of the output (e.g., `PWM_HIGH_SIDE`, `PWM_LOW_SIDE`, `SERVO`, `ON_OFF`).
- **PWM Frequency**: For PWM outputs, defines the frequency in Hz.
- **Curve**: Defines the brightness or speed curve (e.g., `LINEAR`, `LOGARITHMIC`, `EXPONENTIAL`).

### 2.2. Logical Functions

A **Logical Function** represents a specific feature or effect on the locomotive, like "Front Headlight," "Cab Light," or "Smoke Generator." It is an abstract representation of a feature, which is then mapped to one or more Physical Outputs.

Each Logical Function has:
- **Effect Type**: The behavior it generates (e.g., `STEADY`, `FLICKER`, `STROBE`, `SERVO_CONTROL`).
- **Parameters**: A set of values that configure the effect (e.g., brightness for `STEADY`, frequency and intensity for `STROBE`).

### 2.3. Triggers & Function Mapping

**Triggers** are events that activate or deactivate Logical Functions. The **Function Mapping** system is the logic that links these triggers to the functions. This is the heart of the decoder's user-facing configuration.

Triggers can be:
- **Function Keys**: Pressing a function key on the command station (e.g., F0, F1, F5).
- **Decoder State**: Internal decoder states like `DIRECTION_FORWARD`, `DIRECTION_REVERSE`, `IS_MOVING`.
- **Combinations**: Logical combinations of other triggers (e.g., `F1` AND `DIRECTION_FORWARD`).

## 3. Advanced Lighting Effects

This section details the proposed lighting effects. Each effect would be a selectable "Effect Type" for a Logical Function.

### 3.1. Basic Effects

- **Steady**: The output is on at a constant, configurable brightness.
  - *Parameters*: `brightness` (0-255).
- **Dimming**: The brightness of the output can be dynamically adjusted, often by another function key. This is useful for features like "Rule 17" dimming in the US, where headlights are dimmed when a train is stationary in a yard.
  - *Parameters*: `brightness_full` (0-255), `brightness_dimmed` (0-255).

### 3.2. Dynamic & Realistic Effects

- **Flicker/Firebox Glow**: Simulates the flickering of a firebox or an old lantern.
  - *Variant A (Simple)*: Random PWM value changes. Simple to implement.
  - *Variant B (Advanced)*: Uses a Perlin noise or similar algorithm for a more realistic, organic flicker. More computationally intensive but visually superior.
  - **Proposal**: Implement Variant B for the highest realism.
  - *Parameters*: `base_brightness`, `flicker_depth`, `flicker_speed`.
- **Strobe / Beacon**: Simulates a modern strobe light or rotating beacon.
  - *Parameters*: `strobe_frequency` (Hz), `duty_cycle` (%), `brightness`.
- **Neon Tube Simulation**: Simulates the "warm-up" and "cool-down" flicker of a neon or fluorescent light tube.
  - *Implementation*: A state machine that starts with a high-frequency flicker, transitions to a steady state, and has a small random flicker during operation.
  - *Parameters*: `warmup_time` (ms), `steady_brightness`.

### 3.3. Special Effects

- **Mars Light / Gyralite**: Simulates oscillating beam lights.
  - *Implementation*: Uses a sine-wave modulation of the PWM brightness. For dual-output lights (e.g., figure-8 Mars light), two outputs would be driven with phase-shifted sine waves.
  - *Parameters*: `oscillation_frequency` (Hz), `peak_brightness`, `phase_shift` (for dual outputs).
- **Soft Start/Stop**: The light fades in and out smoothly instead of turning on/off instantly. This is particularly effective for incandescent bulb simulations.
  - *Parameters*: `fade_in_time` (ms), `fade_out_time` (ms), `target_brightness`.

### 3.4. Proposal for Implementation

It is proposed to implement all the above effects. The core of the implementation would be a set of C++ classes, each representing an effect type and inheriting from a common `LightEffect` base class. This allows for clean, modular, and extensible code.

```cpp
// Pseudo-code for conceptual illustration
class LightEffect {
public:
    virtual void update(uint32_t delta_ms) = 0;
    virtual uint8_t getPwmValue() = 0;
};

class EffectSteady : public LightEffect {
    // ... implementation
};

class EffectFlicker : public LightEffect {
    // ... implementation
};
```

## 4. Auxiliary Functions (Non-Lighting)

Auxiliary functions cover all non-lighting outputs, providing control for animation and other features.

### 4.1. Servo Control

This is designed for physical animations like automatic couplers, pantographs, or cab doors.

- **Effect Type**: `SERVO_CONTROL`
- **Implementation**: The function triggers a servo to move from a starting angle to an ending angle over a configurable amount of time.
  - *Variant A (Momentary)*: The servo moves to position B while the function key is held, and returns to A when released.
  - *Variant B (Latching)*: The first press of a function key moves the servo from A to B. The second press moves it from B to A.
  - **Proposal**: Implement both variants, selectable via a parameter, as they serve different use cases (e.g., Variant A for manual uncoupling, Variant B for a pantograph).
- **Parameters**:
  - `endpoint_A` (degrees, e.g., 0)
  - `endpoint_B` (degrees, e.g., 90)
  - `travel_speed` (degrees per second)
  - `mode` (`MOMENTARY`, `LATCHING`)

### 4.2. Smoke Generator Control

Modern smoke units often have a heating element and a separate fan. This requires independent control.

- **Effect Type**: `SMOKE_GENERATOR`
- **Implementation**: This logical function would typically be mapped to two physical outputs (one for the heater, one for the fan motor).
  - **Heater**: The heating element is typically a simple on/off control.
  - **Fan**: The fan speed can be synchronized with the locomotive's state to produce realistic smoke puffs.
    - *Variant A (Simple)*: The fan runs at a constant speed when the function is active.
    - *Variant B (Synchronized)*: The fan speed is proportional to the locomotive's speed step. For steam locomotives, this can be further tied to a virtual "chuff" signal to create distinct puffs.
    - **Proposal**: Implement Variant B for maximum realism. A parameter can select between synchronized and static speed.
- **Parameters**:
  - `heater_enabled` (true/false)
  - `fan_mode` (`STATIC`, `SPEED_SYNC`)
  - `static_fan_speed` (0-255, for STATIC mode)
  - `max_fan_speed` (0-255, for SYNC mode)

## 5. Function Mapping System

The function mapping system connects user inputs (triggers) to the logical functions. The architecture is designed to be extremely powerful internally, while providing a standardized and user-friendly configuration interface.

### 5.1. Internal Architecture: Multi-Level Table Logic

The core of the decoder's mapping capability is a powerful and flexible "Multi-level Table Logic" engine. This system uses a set of user-defined "condition variables" (based on function key states, direction, speed, etc.) and a logic table that can evaluate complex boolean expressions (AND/OR/NOT) on these conditions to activate logical functions. This architecture provides the power needed for state-of-the-art features, far exceeding the capabilities of simple mapping systems.

### 5.2. User Configuration: RCN-227 Standard

While the internal engine is proprietary, the user-facing configuration will be implemented via the **RCN-227 standard for Extended Function Mapping**. This approach provides the best of both worlds:

*   **Standardization & Compatibility**: Users can configure the decoder using familiar methods and tools (like JMRI) that support the RCN-227 standard.
*   **Power & Flexibility**: The firmware will translate the standardized RCN-227 CVs into the more powerful rules for the internal engine, unlocking the full potential of the hardware and effects system.

As defined in RCN-225 and RCN-227, the user can select the active mapping system using CV 96. This will allow switching between basic NMRA mapping, the various RCN-227 systems, and potentially a "native" mode to directly access the advanced internal engine in the future.

## 6. Protocol-Specific Considerations

The decoder is intended to be multi-protocol. While the core logic of the effects and mapping system is protocol-agnostic, the triggers available are not.

### 6.1. DCC (Digital Command Control)

The DCC standard is very flexible and is the primary target for this feature set.
- **Functions**: DCC supports a large number of functions (F0-F28 or even higher, depending on the command station). All these function keys can be used as triggers in the mapping table.
- **Configuration**: Configuration is typically done via Configuration Variables (CVs). The RCN-227 standard provides a comprehensive and compatible method for this.

### 6.2. Märklin-Motorola (MM)

The MM protocol is older and more limited.
- **Functions**: The original MM protocol only supports one function (F0, for lights) and four additional functions (F1-F4) in later extensions. This is the primary constraint.
- **Proposal**: The full set of logical functions and effects will still be available for configuration internally. However, only the limited MM function keys can be used as primary triggers in the mapping table.
- **Strategy**: To maximize utility, the user can map the most desired logical functions to the available F0-F4 keys. For example:
  - `F0` could control the direction-dependent headlights.
  - `F1` could activate a complex logical function like "shunting mode" which might change lighting and motor characteristics.
  - `F2` could control the cab light.
  - The powerful mapping table allows a single MM function key to have different effects based on direction or speed, partially compensating for the limited number of keys.
- **Configuration**: Configuration on MM-only systems is more challenging. The initial setup would likely require a DCC system to write the CVs. For users without access to DCC, a simplified, pre-configured "personality" could be selected via a single CV.

## 7. RCN-227 Implementation Roadmap

To manage the complexity of this feature, a phased implementation is planned. This will allow for incremental development and testing, reducing risk and ensuring a robust final product.

### Phase 1: Core Infrastructure & RCN-225

- **Goal**: Refactor the existing mapping system to support the new architecture and implement the baseline RCN-225 standard.
- **Tasks**:
  1.  **Refactor `CVLoader`**: Modify the `CVLoader` to read and parse the CVs required for RCN-225 (CVs 33-46 and CV 96).
  2.  **Refactor `FunctionManager`**: Update the `FunctionManager` to accept mapping rules from the `CVLoader` and populate its internal "Multi-level Table Logic" engine.
  3.  **Implement RCN-225 Logic**: Implement the basic direction-dependent function mapping as described in RCN-225.
  4.  **Unit Tests**: Create a suite of unit tests to verify the RCN-225 implementation.
- **Outcome**: A decoder that correctly implements the standard NMRA/RCN-225 function mapping, with the underlying architecture ready for RCN-227.

### Phase 2: RCN-227 - System per Function

- **Goal**: Implement the first of the advanced RCN-227 mapping systems.
- **Tasks**:
  1.  **Update `CVLoader`**: Extend `CVLoader` to handle the indexed CV access (via CV 31/32) and parse the "per function" mapping table (CV 32 = 40).
  2.  **Implement Mapping Logic**: Add the logic to the `FunctionManager` to translate the RCN-227 "per function" bitmask into the internal rule set. This includes handling the "blocking" function logic.
  3.  **Unit Tests**: Write specific unit tests to verify the "per function" mapping, including various bitmask combinations and the blocking functionality.
- **Outcome**: The decoder now supports the RCN-227 "per function" mapping system, selectable via CV 96.

### Phase 3: RCN-227 - System per Output (Versions 1 & 2)

- **Goal**: Implement the simpler "per output" mapping systems.
- **Tasks**:
  1.  **Update `CVLoader`**: Extend `CVLoader` to parse the "per output" Version 1 (Matrix) and Version 2 (Function Number) tables (CV 32 = 41 and 42).
  2.  **Implement V1 Logic**: Add logic to translate the V1 "Matrix" bitmasks for each output into the internal rule set.
  3.  **Implement V2 Logic**: Add logic to translate the V2 "Function Number" lists (including the "disable" function) into the internal rule set.
  4.  **Unit Tests**: Create comprehensive unit tests for both V1 and V2, covering various function combinations and the V2 disabling logic.
- **Outcome**: The decoder now supports two additional RCN-227 mapping modes.

### Phase 4: RCN-227 - System per Output (Version 3)

- **Goal**: Implement the most powerful and complex RCN-227 mapping system.
- **Tasks**:
  1.  **Update `CVLoader`**: Extend `CVLoader` to parse the "per output" Version 3 (Function or Binary State Number) table (CV 32 = 43).
  2.  **Implement V3 Logic**: This is the most complex step. Implement the logic to handle direction-dependent functions, blocking functions, and the combined function/binary-state numbers. This will map directly to the most powerful features of the internal engine.
  3.  **Unit Tests**: Develop an extensive set of unit tests for V3, covering all directional dependencies, blocking conditions, and complex scenarios described in the RCN-227 examples.
- **Outcome**: The decoder now fully implements the RCN-227 standard.

### Phase 5: Tooling & Documentation

- **Goal**: Improve user-friendliness and finalize documentation.
- **Tasks**:
  1.  **JMRI Definition File**: Create or update a JMRI decoder definition XML file to provide a user-friendly GUI for configuring all the new RCN-227 CVs.
  2.  **User Manual**: Write comprehensive user documentation explaining how to use the new function mapping capabilities.
- **Outcome**: A mature, easy-to-use, and well-documented decoder framework.
