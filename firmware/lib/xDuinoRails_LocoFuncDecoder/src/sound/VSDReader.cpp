#include "VSDReader.h"

size_t VSDReader::read_callback(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n) {
    File* f = (File*)pOpaque;
    if (f->position() != file_ofs) {
        if (!f->seek(file_ofs)) return 0;
    }
    return f->read((uint8_t*)pBuf, n);
}

size_t VSDReader::write_callback(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n) {
    File* f = (File*)pOpaque;
    if (f->position() != file_ofs) {
        if (!f->seek(file_ofs)) return 0;
    }
    return f->write((uint8_t*)pBuf, n);
}

VSDReader::VSDReader() : _is_open(false), _cache_dir("/vsd_cache") {
    mz_zip_zero_struct(&_zip_archive);
}

VSDReader::~VSDReader() {
    end();
}

bool VSDReader::begin(const char* filename) {
    if (_is_open) end();

    _vsd_file = LittleFS.open(filename, "r");
    if (!_vsd_file) return false;

    _zip_archive.m_pRead = read_callback;
    _zip_archive.m_pIO_opaque = &_vsd_file;

    if (!mz_zip_reader_init(&_zip_archive, _vsd_file.size(), 0)) {
        _vsd_file.close();
        return false;
    }

    _is_open = true;

    // Ensure cache directory exists
    if (!LittleFS.exists(_cache_dir)) {
        LittleFS.mkdir(_cache_dir);
    }

    // Extract assets
    return extract_all_assets();
}

void VSDReader::end() {
    if (_is_open) {
        mz_zip_reader_end(&_zip_archive);
        _vsd_file.close();
        _is_open = false;
    }
}

bool VSDReader::extract_all_assets() {
    mz_uint num_files = mz_zip_reader_get_num_files(&_zip_archive);
    for (mz_uint i = 0; i < num_files; i++) {
        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&_zip_archive, i, &stat)) continue;

        if (stat.m_is_directory) continue;

        String fname = stat.m_filename;
        if (fname.endsWith(".wav") || fname.endsWith(".WAV")) {
            // Flatten path
            String flatName = fname;
            flatName.replace("/", "_");
            flatName.replace("\\", "_");

            String targetPath = _cache_dir + "/" + flatName;

            // Check if file exists and has same size
            if (LittleFS.exists(targetPath)) {
                File existing = LittleFS.open(targetPath, "r");
                if (existing) {
                    size_t sz = existing.size();
                    existing.close();
                    if (sz == stat.m_uncomp_size) {
                        continue; // Already cached
                    }
                }
            }

            File target = LittleFS.open(targetPath, "w");
            if (target) {
                mz_zip_reader_extract_to_callback(&_zip_archive, i, write_callback, &target, 0);
                target.close();
            }
        }
    }
    return true;
}

bool VSDReader::get_file_data(const char* filename, uint8_t** data, size_t* size) {
    if (!_is_open) return false;
    *data = (uint8_t*)mz_zip_reader_extract_file_to_heap(&_zip_archive, filename, size, 0);
    return (*data != nullptr);
}

String VSDReader::get_asset_path(const char* filename) {
    // Flatten name to match cache
    String flatName = filename;
    flatName.replace("/", "_");
    flatName.replace("\\", "_");

    String path = _cache_dir + "/" + flatName;
    if (LittleFS.exists(path)) return path;
    return "";
}
