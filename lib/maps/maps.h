#pragma once

#include <file_stream.h>

namespace map { 
    bool load_blocks(const IFileSystem& fileSystem, const BBox& bbox, MemCache& memCache);
};