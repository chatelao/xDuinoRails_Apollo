#ifndef SOUNDDRIVER_H
#define SOUNDDRIVER_H

#include <Arduino.h>

/**
 * @class SoundDriver
 * @brief Abstract base class for a sound output device.
 *
 * This class defines the common interface that all sound hardware drivers
 * must implement. This allows the main application to be decoupled from the
 * specific hardware being used.
 */
class SoundDriver {
public:
    virtual ~SoundDriver() {}

    /**
     * @brief Initializes the sound hardware.
     * @return True on success, false on failure.
     */
    virtual bool begin() = 0;

    /**
     * @brief Plays a sound.
     * @param track A number identifying the sound to be played.
     *              For DFPlayer, this is the track number.
     *              For I2S, this could be an index to an embedded sample.
     */
    virtual void play(uint16_t track) = 0;

    /**
     * @brief Sets the master volume.
     * @param volume The volume level (0-255).
     */
    virtual void setVolume(uint8_t volume) = 0;

    /**
     * @brief This method should be called repeatedly in the main loop
     *        to handle any ongoing sound processing.
     */
    virtual void loop() = 0;
};

#endif // SOUNDDRIVER_H
