# CV Table for the xDuino Locomotive Decoder

This document describes the Configuration Variables (CVs) for the xDuino locomotive decoder. The CVs allow for the adjustment of driving characteristics, address, and other decoder functions.

The structure is divided into two main parts:
1.  **Standard CVs:** These CVs follow the RCN-225 standard, ensuring compatibility and intuitive operation with most digital command stations.
2.  **Proprietary Function Mapping:** This is a powerful, project-specific system for advanced configuration of lighting and auxiliary functions. It offers a high degree of flexibility beyond the standard NMRA function mapping.

---

## Section 1: Standard Configuration Variables (RCN-225)

This section covers the standard CVs for basic decoder setup.

| CV | Name | Description | Value Range | Default | Status |
|----|------|-------------|-------------|---------|--------|
| **1** | **Primary Address** | The short DCC address of the locomotive. | 1-127 | `3` | **Implemented** |
| **2** | **Start Voltage** | Defines the minimum voltage (PWM value) sent to the motor at speed step 1. Useful for smooth starting. | 0-255 | `80` | **Implemented** |
| **3** | **Acceleration Rate** | Defines the time the locomotive takes to accelerate from speed 0 to maximum speed. The value is a multiplier. | 0-255 | `50` | **Implemented** |
| **4** | **Braking Rate** | Defines the time the locomotive takes to decelerate from maximum speed to 0. The value is a multiplier. | 0-255 | `100` | **Implemented** |
| 5 | Maximum Speed | Limits the maximum speed of the locomotive. | 0-255 | `255` | Not Implemented |
| 6 | Mid-Point Speed | Defines a point in the middle of the speed curve for a non-linear characteristic. | 0-255 | `128` | Not Implemented |
| **7** | **Firmware Version** | Version number of the decoder firmware. Read-only. | Read | `1` | **Implemented** |
| **8** | **Manufacturer ID** | Identifier of the manufacturer (NMRA Standard). `165` is reserved for DIY projects. Read-only. | Read | `165` | **Implemented** |
| | | | | | |
| 11 | Packet Time-out | Time in 32ms increments to wait for a valid data packet before stopping the motor. `0` = infinite. | 0-255 | `0` | Not Implemented |
| 12 | Power Source Lock | Bit-field for enabling/disabling supported protocols. See table below. | Bit-field | `3` | Not Implemented |
| 13 | Analog Function Enable 1 | Bit-field for enabling functions F1-F8 in analog mode. | Bit-field | `0` | Not Implemented |
| 14 | Analog Function Enable 2 | Bit-field for enabling F0 (FL) and F9-F12 in analog mode. | Bit-field | `1` | Not Implemented |
| | | | | | |
| 17 | Extended Address (High Byte) | The high byte for long DCC addresses (128-10239). | 192-231 | `192` | Not Implemented |
| 18 | Extended Address (Low Byte) | The low byte for long DCC addresses. | 0-255 | `3` | Not Implemented |
| | | | | | |
| **29** | **Configuration Data 1** | Bit-field for basic settings (direction, speed steps, address mode). | Bit-field | `6` | **Implemented** |

### CV 29 - Configuration Data 1 (Bit-field)

| Bit | Name | Description |
|---|---|---|
| 0 | Locomotive Direction | `0`=Normal, `1`=Reversed |
| 1 | FL Location | `0`=F0 in Speed Packet (14 steps), `1`=F0 in Function Packet (28/128 steps) |
| 2 | Power Source | `0`=Digital Only, `1`=Analog Mode Enabled |
| 3 | RailCom Enable | `0`=Disabled, `1`=Enabled |
| 4 | Speed Table Enable | `0`=Use CVs 2, 5, 6 for curve, `1`=Use custom speed table (CVs 67-94) |
| 5 | Extended Addressing | `0`=Use Primary Address (CV 1), `1`=Use Extended Address (CV 17/18) |
| 6 | *Reserved* | |
| 7 | Accessory Decoder Mode | `0`=Multi-function Decoder, `1`=Accessory Decoder |

### CV 12 - Power Source Lock (Bit-field)

| Bit | Name | Description |
|---|---|---|
| 0 | DCC Enable | `1`=DCC Protocol Enabled |
| 1 | Märklin Motorola (MM) Enable | `1`=MM Protocol Enabled |
| 2-7 | *Reserved* | |

---

## Section 2: Proprietary Advanced Function Mapping

The advanced function mapping is a highly flexible system based on three main components: **Logical Functions**, **Condition Variables**, and **Mapping Rules**. These are configured via dedicated CV blocks.

### 1. Logical Functions (CV 200-455) - **IMPLEMENTED**

Logical functions define *what* the decoder does (e.g., turn on a light, move a servo). Up to 32 logical functions can be configured. Each function occupies a block of 8 CVs.

- **Base CV:** `200`
- **Number of Blocks:** `32`
- **Block Size:** `8 CVs`

| CV (Base + Offset) | Name | Description | Value Range |
|---------------------|------|-------------|-------------|
| **Base + 0** | **Effect Type** | Defines the behavior of the function. | 0-255 |
| Base + 1 | Parameter 1 | First parameter for the chosen effect. | 0-255 |
| Base + 2 | Parameter 2 | Second parameter for the chosen effect. | 0-255 |
| Base + 3 | Parameter 3 | Third parameter for the chosen effect. | 0-255 |
| Base + 4 | Physical Output 1 ID | ID of the first associated output (0-based). | 0-255 |
| Base + 5 | Physical Output 2 ID | ID of the second output (e.g., for smoke generator fan). | 0-255 |
| Base + 6 | (Reserved) | | |
| Base + 7 | (Reserved) | | |

#### Effect Types and their Parameters

| Type ID | Effect | Parameter 1 | Parameter 2 | Parameter 3 |
|---|---|---|---|---|
| 1 | **Steady** | Brightness | - | - |
| 2 | **Dimming** | Brightness (full) | Brightness (dimmed) | - |
| 3 | **Flicker** | Base Brightness | Flicker Depth | Flicker Speed |
| 4 | **Strobe** | Frequency (Hz) | Duty Cycle (%) | Brightness |
| 5 | **Mars Light** | Frequency (mHz) | Peak Brightness | Phase Shift (%) |
| 6 | **Soft Start/Stop** | Fade-in time (ms/2) | Fade-out time (ms/2) | Target Brightness |
| 7 | **Servo** | Endpoint A (degrees) | Endpoint B (degrees) | Movement Speed |
| 8 | **Smoke Generator** | Heater on/off (0/1) | Fan Speed | - |

---

### 2. Condition Variables (CV 500-627) - **IMPLEMENTED**

Condition variables define *when* something should happen. They check the decoder's state (e.g., "Is the train moving forward?", "Is F1 active?"). Up to 32 condition variables can be configured. Each variable occupies a block of 4 CVs.

- **Base CV:** `500`
- **Number of Blocks:** `32`
- **Block Size:** `4 CVs`

| CV (Base + Offset) | Name | Description | Value Range |
|---------------------|------|-------------|-------------|
| **Base + 0** | **Source** | The data source to be checked. | `1-4` |
| Base + 1 | **Comparator** | How the source should be checked. | `1-8` |
| Base + 2 | **Parameter** | The value to compare against. | `0-255` |
| Base + 3 | (Reserved) | | |

#### Sources

| ID | Source | Description |
|---|---|---|
| 1 | **Function Key** | State of a function key (F0-F28). Parameter = Key number. |
| 2 | **Direction** | The decoder's direction of travel. |
| 3 | **Speed** | The decoder's speed. |
| 4 | **Logical Function State** | State of another logical function. Parameter = LF ID (1-32). |

#### Comparators

| ID | Operator | Description |
|---|---|---|
| 1 | `EQ` (Equal) | Is equal to |
| 2 | `NEQ` (Not Equal) | Is not equal to |
| 3 | `GT` (Greater Than) | Is greater than |
| 4 | `LT` (Less Than) | Is less than |
| 5 | `GTE` (Greater Than or Equal) | Is greater than or equal to |
| 6 | `LTE` (Less Than or Equal) | Is less than or equal to |
| 7 | `BIT_AND` | Bitwise AND is not zero |
| 8 | `IS_TRUE` | Is true (value != 0) |

---

### 3. Mapping Rules (CV 700-955) - **IMPLEMENTED**

Mapping rules are the core of the system. They link **Condition Variables** to **Logical Functions**. A rule might be: "IF Condition 1 is true AND Condition 2 is not true, THEN activate Logical Function 5." Up to 64 rules can be configured. Each rule occupies a block of 4 CVs.

- **Base CV:** `700`
- **Number of Blocks:** `64`
- **Block Size:** `4 CVs`

| CV (Base + Offset) | Name | Description | Value Range |
|---------------------|------|-------------|-------------|
| **Base + 0** | **Target Logical Function ID** | ID of the logical function to be controlled (1-32). | `1-32` |
| Base + 1 | **Positive Condition ID** | ID of the condition variable (1-32) that must be TRUE. 0 = ignored. | `0-32` |
| Base + 2 | **Negative Condition ID** | ID of the condition variable (1-32) that must be FALSE. 0 = ignored. | `0-32` |
| Base + 3 | **Action** | What should happen to the logical function. | `1-3` |

#### Actions

| ID | Action | Description |
|---|---|---|
| 1 | `ACTIVATE` | Activates the logical function. |
| 2 | `DEACTIVATE` | Deactivates the logical function. |
| 3 | `SET_DIMMED` | Sets the "dimmed" state of the function (only for `EffectDimming`). |
