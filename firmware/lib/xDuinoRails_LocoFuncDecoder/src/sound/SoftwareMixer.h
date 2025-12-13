#ifndef SOFTWARE_MIXER_H
#define SOFTWARE_MIXER_H

#include "WAVStream.h"
#include <xDuinoRails_DccSounds.h>

#define MAX_CHANNELS 16

class SoftwareMixer {
public:
    SoftwareMixer(SoundController& soundController);
    ~SoftwareMixer();

    // Starts the mixer.
    void begin();

    // Plays a WAV stream on the next available channel.
    // The mixer takes ownership of the stream and will delete it when finished.
    void play(WAVStream* stream);

    // This method should be called repeatedly in the main loop.
    // It mixes audio from all active channels and sends it to the SoundController.
    void update();

private:
    struct Channel {
        WAVStream* stream;
        bool is_active;
    };

    SoundController& _soundController;
    Channel _channels[MAX_CHANNELS];
    int16_t _mix_buffer[256]; // A buffer to hold mixed audio data
};

#endif // SOFTWARE_MIXER_H
