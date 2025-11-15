/**
 * @file WavDecoder.cpp
 * @brief Decodes WAV file data.
 */

#include "WavDecoder.h"
#include <cstring>

// Helper function to read a little-endian uint16_t from a byte buffer
static inline uint16_t read_le16(const uint8_t* p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

// Helper function to read a little-endian uint32_t from a byte buffer
static inline uint32_t read_le32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

WavDecoder::WavDecoder() :
    _sampleRate(0),
    _bitsPerSample(0),
    _numChannels(0),
    _numSamples(0),
    _pcmData(nullptr) {
}

WavDecoder::~WavDecoder() {
    // Destructor implementation
}

bool WavDecoder::load(const uint8_t* data, size_t size) {
    if (size < 44) { // Minimum size for a valid WAV header
        return false;
    }

    // --- Check RIFF header ---
    if (memcmp(data, "RIFF", 4) != 0 || memcmp(data + 8, "WAVE", 4) != 0) {
        return false;
    }

    // --- Find and parse chunks ---
    uint32_t offset = 12; // Start of chunks
    bool found_fmt = false;
    bool found_data = false;

    while (offset < size - 8) {
        const char* chunk_id = (const char*)(data + offset);
        uint32_t chunk_size = read_le32(data + offset + 4);

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            if (chunk_size < 16) return false;

            uint16_t audio_format = read_le16(data + offset + 8);
            if (audio_format != 1) return false; // Must be PCM

            _numChannels   = read_le16(data + offset + 10);
            _sampleRate    = read_le32(data + offset + 12);
            _bitsPerSample = read_le16(data + offset + 22);

            // For now, only support 16-bit mono
            if (_numChannels != 1 || _bitsPerSample != 16) {
                return false;
            }

            found_fmt = true;
        } else if (memcmp(chunk_id, "data", 4) == 0) {
            _pcmData = (const int16_t*)(data + offset + 8);
            _numSamples = chunk_size / (_bitsPerSample / 8);
            found_data = true;
        }

        if (found_fmt && found_data) {
            return true;
        }

        offset += (chunk_size + 8);
        if (offset % 2 != 0) { // chunks are padded to an even boundary
            offset++;
        }
    }

    return false; // Required chunks not found
}
