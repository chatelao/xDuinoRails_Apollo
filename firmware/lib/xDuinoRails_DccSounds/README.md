# xDuinoRails_DccSounds

**xDuinoRails_DccSounds** is a Hardware Abstraction Layer (HAL) library designed for the xDuinoRails project. It provides a unified interface for controlling various sound output hardware, allowing the main firmware to play audio without needing to know the specific details of the underlying device.

## Features

*   **Unified Interface:** A single `SoundController` class manages sound output.
*   **Driver-Based Architecture:** Supports multiple hardware backends via the `SoundDriver` abstract base class.
*   **Compile-Time Configuration:** Select the active driver using preprocessor definitions (build flags) to optimize code size.

## Supported Drivers

The library includes drivers for the following hardware:

1.  **DFPlayer Mini (`DFPlayerDriver`):**
    *   Controls a DFPlayer Mini MP3 module via serial (UART).
    *   Good for playing pre-recorded MP3/WAV files stored on an SD card.
    *   Requires `SoftwareSerial` and `DFMiniMp3` libraries.

2.  **I2S (`I2SDriver`):**
    *   Outputs high-quality digital audio via the I2S protocol.
    *   Designed for the RP2040 microcontroller, utilizing its PIO (Programmable I/O) and DMA (Direct Memory Access) for efficient, non-blocking audio streaming.
    *   Supports raw PCM data streaming.

3.  **PWM (`PWMDriver`):**
    *   Outputs audio using Pulse Width Modulation (PWM) on a digital pin.
    *   Simple, low-cost solution requiring only a basic RC filter.
    *   *Note: Implemented in `PWNDriver.cpp`.*

4.  **PCM (`PCMDriver`):**
    *   Outputs audio using Pulse Code Modulation.
    *   (Details depend on implementation, often similar to PWM or R-2R ladder).

## Configuration

To select a driver, define **one** of the following macros in your `platformio.ini` build flags or `config.h`:

*   `SOUND_DRIVER_DFPLAYER`
*   `SOUND_DRIVER_I2S`
*   `SOUND_DRIVER_PWM`
*   `SOUND_DRIVER_PCM`

### Pin Configuration

The drivers rely on pin definitions, typically expected to be in a global `config.h` file or defined as build flags.

*   **DFPlayer:** `DFPLAYER_RX_PIN`, `DFPLAYER_TX_PIN`
*   **I2S:** `I2S_BCLK_PIN`, `I2S_LRCLK_PIN`, `I2S_DIN_PIN`
*   **PWM:** `PWM_SOUND_PIN`
*   **PCM:** `PCM_SOUND_PIN`

## Usage

### Initialization

```cpp
#include <xDuinoRails_DccSounds.h>

SoundController soundController;

void setup() {
    if (!soundController.begin()) {
        // Handle error
    }
}
```

### Playing Sound

The `SoundController` exposes standard methods to control playback:

```cpp
void loop() {
    // Process any required background tasks for the driver
    soundController.loop();

    // Example: Play track 1
    if (shouldPlaySound) {
        soundController.play(1);
    }
}
```

### Streaming Audio (I2S)

For drivers that support streaming (like I2S), you can write raw audio data:

```cpp
if (soundController.availableForWrite() >= dataSize) {
    soundController.write(audioData, dataSize);
}
```

## Architecture

*   **`SoundController`**: The main facade class. It instantiates the appropriate driver based on the build flags and forwards method calls to it.
*   **`SoundDriver`**: The abstract base class that defines the interface (`begin`, `play`, `setVolume`, `loop`, `write`).
*   **`...Driver`**: Concrete implementations of `SoundDriver` for specific hardware.
