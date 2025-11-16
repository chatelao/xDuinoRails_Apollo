#include "WAVPlayer.h"

WAVPlayer::WAVPlayer(SoundController& soundController) : _soundController(soundController), _data(nullptr), _size(0), _is_playing(false) {
}

WAVPlayer::~WAVPlayer() {
    if (_data) {
        free(_data);
    }
}

bool WAVPlayer::begin(uint8_t* data, size_t size) {
    if (_data) {
        free(_data);
    }
    _data = data;
    _size = size;

    if (_size < sizeof(wav_header_t)) {
        return false;
    }

    memcpy(&_header, _data, sizeof(wav_header_t));

    if (strncmp(_header.chunk_id, "RIFF", 4) != 0 || strncmp(_header.format, "WAVE", 4) != 0) {
        return false;
    }

    _audio_data = _data + sizeof(wav_header_t);
    _audio_data_size = _header.subchunk2_size;
    _playback_position = 0;

    return true;
}

void WAVPlayer::play() {
    _is_playing = true;
    _playback_position = 0;
}

void WAVPlayer::stop() {
    _is_playing = false;
}

bool WAVPlayer::is_playing() {
    return _is_playing;
}

void WAVPlayer::update() {
    if (!_is_playing) {
        return;
    }

    if (_playback_position >= _audio_data_size) {
        _is_playing = false;
        return;
    }

    size_t bytes_to_play = _soundController.availableForWrite();
    if (bytes_to_play > 0) {
        bytes_to_play = min(bytes_to_play, _audio_data_size - _playback_position);
        _soundController.write(_audio_data + _playback_position, bytes_to_play);
        _playback_position += bytes_to_play;
    }
}
