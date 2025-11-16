#ifndef WAV_PLAYER_H
#define WAV_PLAYER_H

#include <Arduino.h>
#include <xDuinoRails_DccSounds.h>

class WAVPlayer {
public:
    WAVPlayer(SoundController& soundController);
    ~WAVPlayer();
    bool begin(uint8_t* data, size_t size);
    void play();
    void stop();
    bool is_playing();
    void update();

private:
    SoundController& _soundController;
    uint8_t* _data;
    size_t _size;
    bool _is_playing;
    uint8_t* _audio_data;
    size_t _audio_data_size;
    size_t _playback_position;

    // WAV header format
    struct wav_header_t {
        char chunk_id[4];
        uint32_t chunk_size;
        char format[4];
        char subchunk1_id[4];
        uint32_t subchunk1_size;
        uint16_t audio_format;
        uint16_t num_channels;
        uint32_t sample_rate;
        uint32_t byte_rate;
        uint16_t block_align;
        uint16_t bits_per_sample;
        char subchunk2_id[4];
        uint32_t subchunk2_size;
    };

    wav_header_t _header;
};

#endif // WAV_PLAYER_H
