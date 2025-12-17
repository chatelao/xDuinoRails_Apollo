# Implementation Status of LIGHT_AND_AUX_CONCEPT.MD

This document outlines the current implementation status of the features described in `./LIGHT_AND_AUX_CONCEPT.MD`.

## 2. Core Concepts

### 2.1. Physical Outputs

- **Status**: Partially Implemented

- **Details**:
    - The `PhysicalOutput` class exists and supports `PWM` and `SERVO` types.
    - The more specific output types (`PWM_HIGH_SIDE`, `PWM_LOW_SIDE`, `ON_OFF`) are **not implemented**.
    - Configurable `PWM Frequency` is **not implemented**.
    - Configurable brightness/speed `Curve` is **not implemented**.

### 2.2. Logical Functions

- **Status**: Implemented

- **Details**:
    - The `LogicalFunction` class is implemented as described in the concept.
    - It successfully decouples the logical feature from the physical outputs by holding a collection of `PhysicalOutput` pointers and an `Effect` pointer.
    - The core logic for activating, deactivating, and updating the function is in place.

## 3. Advanced Lighting Effects

- **Steady**: Implemented (`EffectSteady`)
- **Dimming**: Implemented (`EffectDimming`)
- **Flicker/Firebox Glow**: Implemented (`EffectFlicker`). The proposed advanced variant using Perlin noise is implemented.
- **Strobe / Beacon**: Implemented (`EffectStrobe`)
- **Neon Tube Simulation**: **Not Implemented**
- **Mars Light / Gyralite**: Implemented (`EffectMarsLight`)
- **Soft Start/Stop**: Implemented (`EffectSoftStartStop`)

## 4. Auxiliary Functions

### 4.1. Servo Control

- **Status**: Partially Implemented

- **Details**:
    - A basic servo control effect (`EffectServo`) is implemented.
    - The proposed `MOMENTARY` and `LATCHING` modes are **not implemented**. The current implementation defaults to a latching-style behavior.

### 4.2. Smoke Generator Control

- **Status**: Partially Implemented

- **Details**:
    - The basic `SMOKE_GENERATOR` effect (`EffectSmokeGenerator`) is implemented.
    - It supports on/off control of the heater and a static fan speed (`Variant A`).
    - The advanced `SPEED_SYNC` mode (`Variant B`) is **not implemented**.

## 5. Function Mapping System

### 5.1. Internal Architecture: Multi-Level Table Logic

- **Status**: Implemented

- **Details**:
    - The core data structures (`Condition`, `ConditionVariable`, `MappingRule`) and enums (`TriggerSource`, `TriggerComparator`, `MappingAction`) for the advanced logic engine are fully implemented.
    - The `FunctionManager` is capable of processing these rules.

### 5.2. RCN-227 Implementation Roadmap

- **Phase 1: Core Infrastructure & RCN-225**: Implemented. The `CVLoader` can parse RCN-225 CVs (33-46) and create the corresponding mapping rules.
- **Phase 2: RCN-227 - System per Function**: Implemented. The `CVLoader` has a dedicated function (`parseRcn227PerFunction`) to parse the indexed CVs for this system, including the blocking function logic.
- **Phase 3: RCN-227 - System per Output (Versions 1 & 2)**: Implemented. The `CVLoader` now contains parsing logic for both the V1 (Matrix) and V2 (Function Number) systems. Unit tests have been written but could not be fully executed due to a persistent timeout issue in the native test environment.
- **Phase 4: RCN-227 - System per Output (Version 3)**: **Not Implemented**.
- **Phase 5: Tooling & Documentation**: **Not Implemented**. No JMRI definition file or specific user manual for RCN-227 has been created.

## 6. Summary

The project has a strong foundation that aligns well with the core architectural principles of the concept document. The separation of logical functions from physical outputs is well-established, and the internal "Multi-Level Table Logic" engine is in place.

- **Strengths**: A significant number of the advanced lighting effects have been implemented. The RCN-225 and the first stage of RCN-227 function mapping are complete, providing a solid, standards-compliant base.

- **Areas for Development**: The main areas for future work are the completion of the RCN-227 standard (Phases 3 and 4), the implementation of the more advanced features for auxiliary functions (servo modes and smoke sync), and the enhancement of the `PhysicalOutput` class to support more detailed hardware configurations. The `Neon Tube Simulation` effect is the only lighting effect not yet implemented.
