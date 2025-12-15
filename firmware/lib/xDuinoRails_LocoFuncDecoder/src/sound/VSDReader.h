#ifndef VSD_READER_H
#define VSD_READER_H

#include <Arduino.h>
#include <LittleFS.h>
#include "miniz.h"

class VSDReader {
public:
    VSDReader();
    ~VSDReader();

    // Opens the VSD file and extracts assets to local cache.
    bool begin(const char* filename);
    void end();

    // Reads a file from the VSD (e.g. config.xml) into memory.
    // Caller owns the buffer and must free() it.
    // For audio files, use the cached files on disk.
    bool get_file_data(const char* filename, uint8_t** data, size_t* size);

    // Returns the path to the cached asset file on LittleFS.
    // Returns empty string if not found.
    String get_asset_path(const char* filename);

private:
    mz_zip_archive _zip_archive;
    bool _is_open;
    File _vsd_file;
    String _cache_dir;

    bool extract_all_assets();
    bool extract_file_to_cache(const char* filename);
    static size_t read_callback(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n);
    static size_t write_callback(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n);
};

#endif // VSD_READER_H
