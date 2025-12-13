#include "SoftwareMixer.h"

SoftwareMixer::SoftwareMixer(SoundController& soundController) : _soundController(soundController) {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        _channels[i].stream = nullptr;
        _channels[i].is_active = false;
    }
}

SoftwareMixer::~SoftwareMixer() {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (_channels[i].stream) {
            delete _channels[i].stream;
        }
    }
}

void SoftwareMixer::begin() {
    // Nothing to do here yet
}

void SoftwareMixer::play(WAVStream* stream) {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (!_channels[i].is_active) {
            _channels[i].stream = stream;
            _channels[i].is_active = true;
            return;
        }
    }
    // If no channels were free, the sound is dropped.
    // We must delete the stream to avoid a memory leak.
    delete stream;
}

void SoftwareMixer::update() {
    // This mixing logic is simplified. A real implementation would need to handle
    // different sample rates and bit depths, and use a more sophisticated mixing algorithm.
    // For now, we assume all streams are 16-bit stereo at the same sample rate as the output.

    size_t samples_to_mix = _soundController.availableForWrite() / 4; // 2 channels, 16 bits
    if (samples_to_mix == 0) return;

    samples_to_mix = min(samples_to_mix, sizeof(_mix_buffer) / sizeof(_mix_buffer[0]) / 2);

    // Clear the mix buffer
    memset(_mix_buffer, 0, samples_to_mix * 4);

    // Mix active channels
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (_channels[i].is_active) {
            if (_channels[i].stream->is_finished()) {
                delete _channels[i].stream;
                _channels[i].stream = nullptr;
                _channels[i].is_active = false;
            } else {
                for (size_t j = 0; j < samples_to_mix; ++j) {
                    int16_t left, right;
                    _channels[i].stream->get_next_sample(&left, &right);
                    // Simple additive mixing with clipping
                    int32_t mixed_left = _mix_buffer[j * 2] + left;
                    int32_t mixed_right = _mix_buffer[j * 2 + 1] + right;
                    _mix_buffer[j * 2] = (int16_t)max(min(mixed_left, 32767), -32768);
                    _mix_buffer[j * 2 + 1] = (int16_t)max(min(mixed_right, 32767), -32768);
                }
            }
        }
    }

    // Write the mixed buffer to the sound controller
    _soundController.write((uint8_t*)_mix_buffer, samples_to_mix * 4);
}
