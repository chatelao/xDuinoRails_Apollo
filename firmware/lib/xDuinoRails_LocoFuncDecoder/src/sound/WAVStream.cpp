#include "WAVStream.h"
#include <string.h>

WAVStream::WAVStream()
    : _buffer_head(0), _buffer_tail(0), _buffer_count(0),
      _data_start_offset(0), _data_length(0), _bytes_read_from_file(0),
      _is_looping(false), _finished(true) {
    memset(&_header, 0, sizeof(wav_header_t));
}

WAVStream::~WAVStream() {
    if (_file) {
        _file.close();
    }
}

bool WAVStream::begin(File file) {
    _file = file;
    if (!_file) {
        return false;
    }

    _file.seek(0);
    if (_file.read((uint8_t*)&_header, sizeof(wav_header_t)) != sizeof(wav_header_t)) {
        return false;
    }

    // Basic validation
    if (strncmp(_header.chunk_id, "RIFF", 4) != 0 || strncmp(_header.format, "WAVE", 4) != 0) {
        return false;
    }

    _data_start_offset = sizeof(wav_header_t);
    _data_length = _header.subchunk2_size;
    _bytes_read_from_file = 0;
    _finished = false;

    // We only support PCM audio format (1).
    if (_header.audio_format != 1) return false;

    // Reset buffer
    _buffer_head = 0;
    _buffer_tail = 0;
    _buffer_count = 0;

    // Fill buffer initially
    service();

    return true;
}

void WAVStream::service() {
    if (!_file || _finished) return;

    // While we have space in buffer
    while (_buffer_count < WAV_STREAM_BUFFER_SIZE) {
        // Determine how much we can write to the buffer (linear space at head)
        size_t space_at_end = WAV_STREAM_BUFFER_SIZE - _buffer_head;
        size_t space_total = WAV_STREAM_BUFFER_SIZE - _buffer_count;
        size_t write_len = (space_at_end < space_total) ? space_at_end : space_total;

        // Limit to remaining data in current file pass
        if (_bytes_read_from_file >= _data_length) {
            if (_is_looping) {
                // Loop: Reset to data start
                _bytes_read_from_file = 0;
                _file.seek(_data_start_offset);
            } else {
                // End of file
                if (_buffer_count == 0) _finished = true;
                return;
            }
        }

        size_t bytes_remaining = _data_length - _bytes_read_from_file;
        if (write_len > bytes_remaining) write_len = bytes_remaining;

        // Read from file
        size_t read = _file.read(_buffer + _buffer_head, write_len);
        if (read == 0) {
            // Read error or unexpected EOF
             if (_is_looping) {
                _bytes_read_from_file = 0;
                _file.seek(_data_start_offset);
                continue;
             } else {
                 // Stop filling
                 if (_buffer_count == 0) _finished = true;
                 break;
             }
        }

        _buffer_head = (_buffer_head + read) % WAV_STREAM_BUFFER_SIZE;
        _buffer_count += read;
        _bytes_read_from_file += read;
    }
}

void WAVStream::get_next_sample(int16_t* left, int16_t* right) {
    // Check if we have enough data for a sample
    uint16_t bytes_per_sample = (_header.bits_per_sample / 8) * _header.num_channels;

    if (_buffer_count < bytes_per_sample) {
        // Buffer underrun or finished
        *left = 0;
        *right = 0;
        if (_bytes_read_from_file >= _data_length && !_is_looping && _buffer_count == 0) {
            _finished = true;
        }
        return;
    }

    uint8_t sample_bytes[4]; // Max 2 channels * 16 bits = 4 bytes
    for (int i=0; i<bytes_per_sample; i++) {
        sample_bytes[i] = _buffer[_buffer_tail];
        _buffer_tail = (_buffer_tail + 1) % WAV_STREAM_BUFFER_SIZE;
        _buffer_count--;
    }

    if (_header.bits_per_sample == 16) {
        if (_header.num_channels == 2) {
            *left = *((int16_t*)(sample_bytes));
            *right = *((int16_t*)(sample_bytes + 2));
        } else { // Mono
            *left = *right = *((int16_t*)(sample_bytes));
        }
    } else if (_header.bits_per_sample == 8) {
        if (_header.num_channels == 2) {
            *left = (sample_bytes[0] - 128) << 8;
            *right = (sample_bytes[1] - 128) << 8;
        } else { // Mono
            *left = *right = (sample_bytes[0] - 128) << 8;
        }
    }
}

bool WAVStream::is_finished() const {
    return _finished;
}

void WAVStream::rewind() {
    if (_file) {
        _file.seek(_data_start_offset);
        _bytes_read_from_file = 0;
        _buffer_head = 0;
        _buffer_tail = 0;
        _buffer_count = 0;
        _finished = false;
        service(); // Refill immediately
    }
}

void WAVStream::setLooping(bool looping) {
    _is_looping = looping;
}

uint32_t WAVStream::get_sample_rate() const {
    return _header.sample_rate;
}

uint16_t WAVStream::get_num_channels() const {
    return _header.num_channels;
}

uint16_t WAVStream::get_bits_per_sample() const {
    return _header.bits_per_sample;
}

size_t WAVStream::get_total_samples() const {
    if (_header.block_align == 0) return 0;
    return _data_length / _header.block_align;
}
