#pragma once
#include <Arduino.h>
#include <xDuinoRails_DccLightsAndFunctions.h>

struct LocoFuncDecoderConfig {
    // --- Feature Enable Flags ---
    bool enableMotor = false;
    bool enableSound = false;
    bool enableLights = false;

    // --- Motor Driver Pins ---
    int motorPinA = -1;
    int motorPinB = -1;
    int bemfPinA = -1;
    int bemfPinB = -1;
    int startupKickDuration = 10;

    // --- Sound Driver Pins (I2S) ---
    int i2sBclkPin = 2;
    int i2sLrclkPin = 3;
    int i2sDinPin = 4;

    // --- Sound Driver Pins (Other - if needed later) ---
    // ...

    // --- Physical Output Configuration ---
    // For simplicity, we can pass a function or use a vector if STL is available.
    // But since this is a library, let's allow the user to access the AuxController directly
    // after begin, or use a setup callback?
    // "Simple Options": The user might want us to set up defaults.
    bool useDefaultPinout = false;

    // Default Pinout (matches original config.h but user can override)
    // Note: These defaults had conflicts in the original project.
    // The user must resolve conflicts if enabling all features.
    int pinHeadlightFwd = 26;
    int pinHeadlightRev = 27;
    int pinCabinLight = 28;
    int pinServo1 = 29;

    // --- Protocol Config (State only, logic is compile-time selected for now) ---
    // The library assumes the user has configured the build flags or #defines for the protocol.
};
