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

The function mapping system connects the triggers (function keys, decoder state) to the logical functions. This system needs to be extremely flexible to accommodate the diverse operational schemes model railroaders use.

### 5.1. Mapping Table Concept

The core of the system is a **Mapping Table**. Each row in the table defines a rule that activates a logical function. The decoder continuously evaluates this table.

A single mapping table row would contain:
- **Conditions**: A set of one or more conditions that must be true for the rule to be active.
- **Target Logical Function**: The logical function to activate.
- **Action**: What to do to the logical function (e.g., `TURN_ON`, `SET_BRIGHTNESS`, `TRIGGER_SERVO`).

### 5.2. Conditions

Conditions are the building blocks of the mapping logic.

- **Function Key State**: `F_STATE(_key_, _state_)` (e.g., `F_STATE(1, ON)`, `F_STATE(5, OFF)`).
- **Direction**: `DIRECTION_IS(_dir_)` (e.g., `DIRECTION_IS(FWD)`, `DIRECTION_IS(REV)`).
- **Speed**: `SPEED_IS(_comparison_, _value_)` (e.g., `SPEED_IS(ZERO)`, `SPEED_IS(GT, 0)` for "is moving").
- **Output State**: `LOGICAL_FUNC_STATE(_func_, _state_)` (e.g., check if another function is already active).

### 5.3. Variants for Mapping Logic

#### Variant A: Simple "AND" Logic (NMRA Style)

This is the traditional approach, similar to the NMRA's extended function mapping CVs. Each mapping rule is a simple AND combination of conditions.

*Example*: Activate "Front Headlight" IF `F0 is ON` AND `Direction is FWD`.
*Pros*: Simple to understand and configure via CVs.
*Cons*: Not very flexible. Cannot easily create "OR" logic or more complex scenarios.

#### Variant B: Scripted Logic (Advanced)

This approach uses a simple scripting language (like Lua) or a custom bytecode interpreter. The user could write small scripts to define the logic.

*Example*: `if f_state(0) == ON and direction() == FWD then set_func("Front Headlight", ON) end`
*Pros*: Extremely powerful and flexible.
*Cons*: Very complex to implement and potentially difficult for non-technical users to configure. Overkill for most use cases.

#### Variant C: Multi-level Table Logic (Proposed)

This is a hybrid approach that offers a good balance of power and usability. It uses a set of "condition variables" and a final logic table.

1.  **Condition Variables (CVs)**: The user first defines a set of boolean "condition variables" based on the basic triggers. For example:
    - `C1 = F_STATE(1, ON)`
    - `C2 = DIRECTION_IS(FWD)`
    - `C3 = SPEED_IS(GT, 0)`
2.  **Logic Table**: The mapping table then uses boolean logic on these condition variables.
    - `IF (C1 AND C2 AND NOT C3) THEN Activate "Yard Mode Light"`

*Pros*: Far more flexible than Variant A. Can create complex AND/OR/NOT logic. Still configurable via a structured table (CVs or a GUI). Avoids the complexity of full scripting.
*Cons*: Requires more configuration steps than Variant A.

### 5.4. Final Proposal

**Variant C is the clear winner.** It provides the power needed for state-of-the-art features without introducing the steep learning curve of a scripting language. This system would be configured as a series of CVs representing the condition definitions and the final logic table entries.

## 6. Protocol-Specific Considerations

The decoder is intended to be multi-protocol. While the core logic of the effects and mapping system is protocol-agnostic, the triggers available are not.

### 6.1. DCC (Digital Command Control)

The DCC standard is very flexible and is the primary target for this feature set.
- **Functions**: DCC supports a large number of functions (F0-F28 or even higher, depending on the command station). All these function keys can be used as triggers in the mapping table.
- **Configuration**: Configuration is typically done via Configuration Variables (CVs). The proposed mapping system (Variant C) can be implemented entirely through CVs. A future PC-based configuration tool could also be used to provide a more user-friendly interface.

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

## 7. Proposed Implementation Roadmap

To make this large project manageable, a phased implementation is recommended.

### Phase 1: Core Infrastructure

- **Goal**: Establish the foundational software components.
- **Tasks**:
  1. Implement the core classes for Physical Outputs and Logical Functions.
  2. Implement the `LightEffect` base class and the simplest effects: `Steady` and `Dimming`.
  3. Implement a basic, direct mapping system (a simplified version of Variant A) to allow F-keys to turn logical functions on and off.
- **Outcome**: A working system with basic, remappable function outputs.

### Phase 2: Advanced Effects & Mapping

- **Goal**: Implement the full power of the proposed mapping system and lighting effects.
- **Tasks**:
  1. Implement the full, multi-level table logic for function mapping (Variant C).
  2. Implement all remaining advanced lighting effects (Flicker, Strobe, Mars Light, etc.).
  3. Develop a comprehensive CV table to configure all new features.
- **Outcome**: A decoder with state-of-the-art lighting capabilities.

### Phase 3: Auxiliary Functions

- **Goal**: Add support for non-lighting hardware.
- **Tasks**:
  1. Implement the `SERVO_CONTROL` effect.
  2. Implement the `SMOKE_GENERATOR` effect, including speed-synchronized fan control.
  3. Add CVs for configuring these new auxiliary functions.
- **Outcome**: A full-featured decoder capable of handling complex animations and smoke effects.

### Phase 4: Tooling & Refinement

- **Goal**: Improve user-friendliness and long-term maintainability.
- **Tasks**:
  1. Develop a PC-based configuration tool to simplify the CV programming process.
  2. Write comprehensive user documentation for all features.
  3. Refine and optimize the code based on community feedback.
- **Outcome**: A mature, easy-to-use, and well-documented decoder framework.
