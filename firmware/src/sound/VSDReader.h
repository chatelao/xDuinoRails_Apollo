#ifndef VSD_READER_H
#define VSD_READER_H

#include <LittleFS.h>
#include "miniz.h"

class VSDReader {
public:
    VSDReader();
    bool begin(const char* filename);
    void end();
    bool get_file_data(const char* filename, uint8_t** data, size_t* size);

private:
    mz_zip_archive _zip_archive;
    bool _is_open;
    uint8_t* _zip_buffer;
};

#endif // VSD_READER_H
