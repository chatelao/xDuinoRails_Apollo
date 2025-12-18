# Technical Debts

A list of known technical debts will be maintained here.

## Documentation
- The following documentation files have been created as placeholders to conform to the project's standards, but they require content:
  - `./CONTRIBUTE.MD`
  - `../docs/HOW_TO_USE.MD`
  - `../docs/USER_MANUAL.MD`
  - `./CORE_CONCEPTS.MD`
  - `./DEVELOPER_REFERENCE.MD`

## Firmware
- `firmware/src/motor_control_hal.h` is a non-functional stub. It was created to resolve a build dependency in the `XDuinoRails_MotorControl` library, which requires a hardware abstraction layer when compiled for the RP2040. This file needs a proper implementation to enable motor control functionality.
- The native unit test environment (`env:native` in `platformio.ini`) consistently times out, preventing unit tests from being run. This issue needs to be investigated and resolved to enable a proper testing workflow.
