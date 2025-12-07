#ifndef WAV_STREAM_H
#define WAV_STREAM_H

#include <Arduino.h>

class WAVStream {
public:
    WAVStream();
    ~WAVStream();

    // Parses the WAV header from the provided data buffer.
    // The buffer is NOT copied, so the caller must ensure it remains valid.
    bool begin(uint8_t* data, size_t size);

    // Gets the next audio sample. Samples are returned as signed 16-bit integers.
    // Mono samples will be duplicated to both left and right channels.
    void get_next_sample(int16_t* left, int16_t* right);

    // Returns true if the end of the audio data has been reached.
    bool is_finished() const;

    // Resets the playback position to the beginning of the audio data.
    void rewind();

    // Enables or disables looping playback.
    void setLooping(bool looping);

    // Public accessors for WAV properties
    uint32_t get_sample_rate() const;
    uint16_t get_num_channels() const;
    uint16_t get_bits_per_sample() const;
    size_t get_total_samples() const;

private:
    uint8_t* _data;
    size_t _size;
    uint8_t* _audio_data;
    size_t _audio_data_size;
    size_t _playback_position; // Position in bytes
    bool _is_looping;

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

#endif // WAV_STREAM_H
