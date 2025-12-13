#include "WAVStream.h"
#include <string.h>

WAVStream::WAVStream() : _data(nullptr), _size(0), _audio_data(nullptr), _audio_data_size(0), _playback_position(0), _is_looping(false) {
    memset(&_header, 0, sizeof(wav_header_t));
}

WAVStream::~WAVStream() {
    if (_data) {
        free(_data);
    }
}

bool WAVStream::begin(uint8_t* data, size_t size) {
    _data = data;
    _size = size;

    if (_size < sizeof(wav_header_t)) {
        return false;
    }

    memcpy(&_header, _data, sizeof(wav_header_t));

    // Basic validation
    if (strncmp(_header.chunk_id, "RIFF", 4) != 0 || strncmp(_header.format, "WAVE", 4) != 0) {
        return false;
    }

    // This is a simplified parser. It assumes the 'data' chunk is the last one.
    // A more robust parser would search for the 'data' chunk ID.
    _audio_data = _data + sizeof(wav_header_t);
    _audio_data_size = _header.subchunk2_size;
    _playback_position = 0;

    // We only support PCM audio format.
    return _header.audio_format == 1;
}

void WAVStream::get_next_sample(int16_t* left, int16_t* right) {
    if (is_finished()) {
        *left = 0;
        *right = 0;
        return;
    }

    // Handle Looping: If we are at the end and looping is enabled, reset to start.
    // Note: We do this check before reading. If _playback_position was incremented
    // to match _audio_data_size in the previous call, we wrap around now.
    if (_playback_position >= _audio_data_size && _is_looping) {
        _playback_position = 0;
    }

    // Double check if we are finished (non-looping case or empty file)
    if (_playback_position >= _audio_data_size) {
        *left = 0;
        *right = 0;
        return;
    }

    if (_header.bits_per_sample == 16) {
        if (_header.num_channels == 2) {
            *left = *((int16_t*)(_audio_data + _playback_position));
            *right = *((int16_t*)(_audio_data + _playback_position + 2));
            _playback_position += 4;
        } else { // Mono
            *left = *right = *((int16_t*)(_audio_data + _playback_position));
            _playback_position += 2;
        }
    } else if (_header.bits_per_sample == 8) {
        if (_header.num_channels == 2) {
            // Convert unsigned 8-bit sample to signed 16-bit
            *left = (*((uint8_t*)(_audio_data + _playback_position)) - 128) << 8;
            *right = (*((uint8_t*)(_audio_data + _playback_position + 1)) - 128) << 8;
            _playback_position += 2;
        } else { // Mono
            *left = *right = (*((uint8_t*)(_audio_data + _playback_position)) - 128) << 8;
            _playback_position += 1;
        }
    } else {
        // Unsupported format
        *left = 0;
        *right = 0;
        // Advance position to avoid infinite loop
        _playback_position = _audio_data_size;
    }
}

bool WAVStream::is_finished() const {
    if (_is_looping) {
        return false;
    }
    return _playback_position >= _audio_data_size;
}

void WAVStream::rewind() {
    _playback_position = 0;
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
    return _audio_data_size / _header.block_align;
}
