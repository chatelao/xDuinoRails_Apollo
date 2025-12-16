#ifndef WAV_STREAM_H
#define WAV_STREAM_H

#include <Arduino.h>
#include <LittleFS.h>

#define WAV_STREAM_BUFFER_SIZE 1024

class WAVStream {
public:
    WAVStream();
    ~WAVStream();

    // Initializes the stream with a file from LittleFS.
    bool begin(File file);

    // Refills the internal buffer from the file. Must be called frequently.
    void service();

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
    File _file;

    // Ring Buffer
    uint8_t _buffer[WAV_STREAM_BUFFER_SIZE];
    size_t _buffer_head; // Write index
    size_t _buffer_tail; // Read index
    size_t _buffer_count; // Number of bytes in buffer

    size_t _data_start_offset; // File offset where audio data begins
    size_t _data_length;       // Total bytes of audio data (from header)
    size_t _bytes_read_from_file; // Tracker for total bytes read (to detect end of data chunk)

    bool _is_looping;
    bool _finished;

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
