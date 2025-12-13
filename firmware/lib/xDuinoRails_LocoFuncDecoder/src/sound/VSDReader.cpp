#include "VSDReader.h"

VSDReader::VSDReader() : _is_open(false), _zip_buffer(nullptr) {
    memset(&_zip_archive, 0, sizeof(_zip_archive));
}

bool VSDReader::begin(const char* filename) {
    if (_is_open) {
        end();
    }

    File file = LittleFS.open(filename, "r");
    if (!file) {
        return false;
    }

    size_t file_size = file.size();
    _zip_buffer = new uint8_t[file_size];
    if (!_zip_buffer) {
        file.close();
        return false;
    }

    if (file.read(_zip_buffer, file_size) != file_size) {
        file.close();
        delete[] _zip_buffer;
        _zip_buffer = nullptr;
        return false;
    }

    file.close();

    if (!mz_zip_reader_init_mem(&_zip_archive, _zip_buffer, file_size, 0)) {
        delete[] _zip_buffer;
        _zip_buffer = nullptr;
        return false;
    }

    _is_open = true;
    return true;
}

void VSDReader::end() {
    if (_is_open) {
        mz_zip_reader_end(&_zip_archive);
        delete[] _zip_buffer;
        _zip_buffer = nullptr;
        _is_open = false;
    }
}

bool VSDReader::get_file_data(const char* filename, uint8_t** data, size_t* size) {
    if (!_is_open) {
        return false;
    }

    int file_index = mz_zip_reader_locate_file(&_zip_archive, filename, nullptr, 0);
    if (file_index < 0) {
        return false;
    }

    mz_zip_archive_file_stat file_stat;
    if (!mz_zip_reader_file_stat(&_zip_archive, file_index, &file_stat)) {
        return false;
    }

    *size = file_stat.m_uncomp_size;
    *data = (uint8_t*)malloc(*size);
    if (!*data) {
        return false;
    }

    if (!mz_zip_reader_extract_to_mem(&_zip_archive, file_index, *data, *size, 0)) {
        free(*data);
        *data = nullptr;
        return false;
    }

    return true;
}
