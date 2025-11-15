/**
 * @file WavDecoder.h
 * @brief Decodes WAV file data.
 */

#ifndef WAVDECODER_H
#define WAVDECODER_H

#include <cstdint>
#include <cstddef>

class WavDecoder {
public:
    WavDecoder();
    ~WavDecoder();

    bool load(const uint8_t* data, size_t size);
    uint32_t getSampleRate() const { return _sampleRate; }
    uint16_t getBitsPerSample() const { return _bitsPerSample; }
    uint16_t getNumChannels() const { return _numChannels; }
    uint32_t getNumSamples() const { return _numSamples; }
    const int16_t* getPcmData() const { return _pcmData; }

private:
    uint32_t _sampleRate;
    uint16_t _bitsPerSample;
    uint16_t _numChannels;
    uint32_t _numSamples;
    const int16_t* _pcmData;
};

#endif // WAVDECODER_H
