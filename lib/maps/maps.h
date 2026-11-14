#pragma once

#include <vector>
#include <file_stream.h>

#define MAPBLOCKS_MAX       6   // max blocks in memory
#define MAPBLOCK_SIZE_BITS 12   // 4096 x 4096 coords (~meters) per block  
#define MAPFOLDER_SIZE_BITS 4   // 16 x 16 map blocks per folder

const int32_t MAPBLOCK_MASK  = pow(2, MAPBLOCK_SIZE_BITS) - 1;     // ...00000000111111111111
const int32_t MAPFOLDER_MASK = pow(2, MAPFOLDER_SIZE_BITS) - 1;    // ...00001111

namespace map { 
    bool load_blocks(const IFileSystem& fileSystem, const BBox& bbox, MemCache& memCache);
};