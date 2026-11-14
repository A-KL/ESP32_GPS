#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <logs.h>
    #include <cassert>
#endif

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <file_stream.h>
#include <geometry.h>

#include "maps.h"

#define MAPBLOCKS_MAX       6   // max blocks in memory
#define MAPBLOCK_SIZE_BITS 12   // 4096 x 4096 coords (~meters) per block  
#define MAPFOLDER_SIZE_BITS 4   // 16 x 16 map blocks per folder

const int32_t MAPBLOCK_MASK  = pow(2, MAPBLOCK_SIZE_BITS) - 1;     // ...00000000111111111111
const int32_t MAPFOLDER_MASK = pow(2, MAPFOLDER_SIZE_BITS) - 1;    // ...00001111

template <typename T = int16_t>
bool read_int(IReadStream* file, T& result)
{
    char num[32] = {0};
    uint8_t i = 0;

    char c = (char) file->read();
    if (c == '\n') {
        log_e("read_int - end of line");
        return false;
    }

    while (c>='0' && c <= '9') {
        assert( i < 15);
        num[i++] = c;
        c = (char) file->read();
    }

    num[i] = '\0';

    if (c != ';' && c != ',' && c != '\n'){
        log_e("read_int - error: %c %i [%s]\n", c, c, num);
        return false;
    }

    try
    {
        result = (T)std::stoi(num);
        return true;
    } catch (std::invalid_argument) {
        log_e("read_int - invalid_argument: [%c] [%s]\n", c, num);
    } catch (std::out_of_range) {
        log_e("read_int - out_of_range: [%c] [%s]\n", c, num);
    }
    return false;
}

// @brief Returns the string until terminator char or newline. The terminator character is not included but comsumed from stream.
void read_str_until(IReadStream* file, char terminator, char *str)
{
    uint8_t i = 0;
    char c = (char) file->read();  
    while (c != terminator && c != '\n') {
        assert(i < 29);
        str[i++] = c;
        c = (char) file->read();
    }
    str[i] = '\0';
}

void read_coords(IReadStream* file, std::vector<Point16>& points)
{
    assert(points.size() == 0);

    char str[32] = {0};
    Point16 point;

    while(true)
    {
       try 
       {
            // X
            read_str_until(file, ',', str);
            if (str[0] == '\0') break;
            point.x = (int16_t) std::atol(str);
            // Y
            read_str_until(file, ';', str);
            assert(str[0] != '\0');
            point.y = (int16_t)std::atol(str);
            //log_d("point: %i %i", point.x, point.y);
        } catch(const std::invalid_argument &error){
            log_e("read_coords - invalid_argument: %s\n", str);
        } catch(const std::out_of_range &error){
            log_e("read_coords - out_of_range: %s\n", str);
        }
        points.push_back(point);
    }
    // points.shrink_to_fit();
}

bool read_polygons(IReadStream* file, std::vector<Polygon>& polygons, int max_zoom)
{  
    char str[64] = {0};
    int16_t count = 0;
    
    read_str_until(file, ':', str);

    if (strcmp(str, "Polygons") != 0) {
        log_e("Map error. Expected Polygons instead of: %s\n", str);
        return false;
    }

    if (!read_int(file, count)) {
        log_e("Error reading polygon count\n");
        return false;
    }
    log_d("Polygons Count: %d\n", count);

    uint32_t total_points = 0;
    Polygon polygon;
    // Point16 p;
    
    while (count > 0) 
    {
        read_str_until(file, '\n', str); // color
        assert(str[0] == '0' && str[1] == 'x');
        polygon.color = (uint16_t)std::stoi(str, nullptr, 16);
        //log_d("polygon.color: %i\n", polygon.color);

        read_str_until(file, '\n', str); // maxzoom
        polygon.max_zoom = str[0] ? (uint8_t)std::stoi(str) : max_zoom;
        // log_d("polygon.maxzoom: %i", polygon.maxzoom);

        read_str_until(file, ':', str);
        if (strcmp(str, "bbox") != 0){
            log_e("bbox error tag. Line: %s\n", str);
            return false;
        }

        assert(read_int<int32_t>(file, polygon.bbox.min.x));
        assert(read_int<int32_t>(file, polygon.bbox.min.y));
        assert(read_int<int32_t>(file, polygon.bbox.max.x));
        assert(read_int<int32_t>(file, polygon.bbox.max.y));

        polygon.points.clear();

        read_str_until(file, ':', str);
        if (strcmp( str, "coords") != 0){
            log_e("coords error tag. Line: %s\n", str);
            return false;
        }
        read_coords(file, polygon.points);
        polygons.push_back(polygon);
        total_points += polygon.points.size();
        count--;
    }
    assert (count == 0);
    
    log_d("Polygons points: %d\n", total_points);

    return true;
}

bool read_polylines(IReadStream* file, std::vector<Polyline>& polylines, int max_zoom)
{  
    char str[64] = {0};
    int16_t count = 0;
    
    read_str_until(file, ':', str);

    if (strcmp(str, "Polylines") != 0) { 
        log_e("Map error. Expected Polylines instead of: %s\n", str);
        return false;
    }

    if (!read_int(file, count)) {
        log_e("Error reading polyline count\n");
        return false;
    }
    log_d("Polylines Count: %d\n", count);
    
    Polyline polyline;
    uint32_t total_points = 0;

    while (count > 0) 
    {
        read_str_until(file, '\n', str);
        assert(str[0] == '0' && str[1] == 'x');
        polyline.color = (uint16_t)std::stoul(str, nullptr, 16);

        read_str_until( file, '\n', str);
        polyline.width = str[0] ? (uint8_t)std::stoi(str) : 1;

        read_str_until( file, '\n', str);
        polyline.max_zoom = str[0] ? (uint8_t)std::stoi(str) : max_zoom;

        read_str_until( file, ':', str);

        if (strcmp(str, "bbox") != 0){
            log_e("bbox error tag. Line: %s\n", str);
            return false;
        }

        assert(read_int<int32_t>(file, polyline.bbox.min.x));
        assert(read_int<int32_t>(file, polyline.bbox.min.y));
        assert(read_int<int32_t>(file, polyline.bbox.max.x));
        assert(read_int<int32_t>(file, polyline.bbox.max.y));

        polyline.points.clear();

        read_str_until(file, ':', str);
        if (strcmp( str, "coords") != 0){
            log_d("coords tag. Line: %s\n", str);
            return false;
        }

        read_coords(file, polyline.points);
        polylines.push_back(polyline);
        total_points += polyline.points.size();
        count--;
    }
    assert (count == 0);
    log_d("Polygons points: %d\n", total_points);

    return true;
}

bool read_map_block(IReadStream* file, MapBlock* result, int max_zoom = 7)
{  
    return 
        read_polygons(file, result->polygons, max_zoom) && 
        read_polylines(file, result->polylines, max_zoom);
}

namespace map {

bool load_blocks(const IFileSystem& fileSystem, const BBox& bbox, MemCache& memCache)
{
    static auto start = millis();

    for (auto block : memCache.blocks){
        block->inView = false;
    }

    // loop the 4 corners of the bbox and find the files that contain them
    for (const auto& point : { bbox.min, bbox.max, Point32(bbox.min.x, bbox.max.y), Point32( bbox.max.x, bbox.min.y) })
    {
        bool found = false;
        int32_t block_min_x = point.x & (~MAPBLOCK_MASK);
        int32_t block_min_y = point.y & (~MAPBLOCK_MASK);
        
        // check if the needed block is already in memory
        for (auto memblock : memCache.blocks) {
            if( block_min_x == memblock->offset.x && block_min_y == memblock->offset.y){
                memblock->inView = true;
                found = true;
                break;
            }
        }

        if (found) {
            continue;
        }
        
        // log_d("load from disk (%i, %i) %i\n", block_min_x, block_min_y, millis());
        // block is not in memory => load from disk
        int32_t block_x = (block_min_x >> MAPBLOCK_SIZE_BITS) & MAPFOLDER_MASK;
        int32_t block_y = (block_min_y >> MAPBLOCK_SIZE_BITS) & MAPFOLDER_MASK;
        int32_t folder_name_x = block_min_x >> (MAPFOLDER_SIZE_BITS + MAPBLOCK_SIZE_BITS);
        int32_t folder_name_y = block_min_y >> (MAPFOLDER_SIZE_BITS + MAPBLOCK_SIZE_BITS);

        char file_name[128] = {0};
        snprintf(file_name, sizeof(file_name), "%+04d%+04d/%d_%d.fmp", folder_name_x, folder_name_y, block_x, block_y);
        log_i("Reading file: '%s'\n", file_name);

        // check if cache is full
        if (memCache.blocks.size() >= MAPBLOCKS_MAX)
        {
            // remove first one, the oldest
            log_d("Deleteing freeHeap: %i\n", esp_get_free_heap_size());

            auto first_block = memCache.blocks.front();
            delete first_block;
            memCache.blocks.erase(memCache.blocks.begin()); // remove pointer from the vector
            
            log_d("Deleted freeHeap: %i\n", esp_get_free_heap_size());
        }

        auto stream = fileSystem.OpenRead(file_name);
        if (!stream) {
            log_e("Map file not found: %s\n", file_name);
            return false;
        }

        auto new_block = new MapBlock();

        if (!read_map_block(stream, new_block)) {
            delete stream;
            delete new_block;
            return false;
        }

        delete stream;

        new_block->inView = true;
        new_block->offset = Point32(block_min_x, block_min_y);
        memCache.blocks.push_back(new_block);

        assert(memCache.blocks.size() <= MAPBLOCKS_MAX);

        log_d("Block read from SD card: %p\n", new_block);
        log_d("FreeHeap: %i\n", esp_get_free_heap_size());
    }   

    log_d("memCache size: %i\n", memCache.blocks.size());
    log_d("get_map_blocks %i\n", millis()-start);

    return true;
}

}