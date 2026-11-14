#pragma once

#include <vector>
#include <geometry.h>

#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <logs.h>
    #include <cassert>
#endif

#include "maps.h"
#include "string_reader.h"

class MapReader {
  public:
    MapReader(StringReader* stream) : _stream(stream)
    { }

    void ReadCoords(std::vector<Point16>& points)
    {
        assert(points.size() == 0);

        char str[64];
        Point16 point;

        while(true)
        {
          try 
          {
            _stream->ReadUntil(',', str);
            if (str[0] == '\0') break;
            point.x = (int16_t) std::atol(str);

            _stream->ReadUntil(';', str);
            assert(str[0] != '\0');
            point.y = (int16_t)std::atol(str);

            // log_d("x: %i, y: %s, ram: %i", point.x, str, esp_get_free_heap_size() / 1024);
            // log_d("point: %i %i", point.x, point.y);

            } catch(const std::invalid_argument &error){
                log_e("read_coords - invalid_argument: %s\n", str);
            } catch(const std::out_of_range &error){
                log_e("read_coords - out_of_range: %s\n", str);
            }
            points.push_back(point);
        }
        // points.shrink_to_fit();
    }

    bool ReadBlock(MapBlock& result, int max_zoom = 7)
    {  
        Polygon polygon;
        Point16 p;
        int16_t maxzoom;

        uint32_t total_points = 0;
        uint32_t line = 0;

        char str[64] = {0};

        _stream->ReadUntil(':', str);
        if (strcmp(str, "Polygons") != 0) {
            log_e("Map error. Expected Polygons instead of: %s\n", str);
            return false;
        }

        auto count = read_int16(file);
        if (count <= 0) {
            log_e("Error reading polygon count\n");
            return false;
        }

        line++;
        log_d("Polygon count: %i\n", count);
    
        while (count > 0) {
            // log_d("line: %i", line);
            _stream->ReadUntil('\n', str); // color
            assert( str[0] == '0' && str[1] == 'x');
            polygon.color = (uint16_t)std::stoi(str, nullptr, 16);
            //log_d("polygon.color: %i\n", polygon.color);
            line++;
            _stream->ReadUntil('\n', str); // maxzoom
            polygon.maxzoom = str[0] ? (uint8_t )std::stoi( str) : max_zoom;
            // log_d("polygon.maxzoom: %i", polygon.maxzoom);
            line++;

            _stream->ReadUntil(':', str);

            if (strcmp(str, "bbox") != 0){
                log_e("bbox error tag. Line %i : %s\n", line, str);
                return false;
            }

            polygon.bbox.min.x = read_int16( file);
            polygon.bbox.min.y = read_int16( file);
            polygon.bbox.max.x = read_int16( file);
            polygon.bbox.max.y = read_int16( file);

            line++;

            polygon.points.clear();
            _stream->ReadUntil(':', str);
            if (strcmp( str, "coords") != 0){
                log_e("coords error tag. Line %i : %s\n", line, str);
                return false;
            }

            ReadCoords(polygon.points);
            line++;
            result.polygons.push_back(polygon);
            total_points += polygon.points.size();
            count--;
        }
        assert(count == 0);
    
        // read lines
        _stream->ReadUntil(':', str);

        if (strcmp(str, "Polylines") != 0) 
            log_e("Map error. Expected Polylines instead of: %s\n", str);

        count = read_int16(file);
        assert( count > 0);
        line++;

        log_d("Polylines Count: %i", count);
              
        Polyline polyline;

            while (count > 0) 
            {
                _stream->ReadUntil('\n', str); // color
                assert(str[0] == '0' && str[1] == 'x');
                polyline.color = (uint16_t)std::stoul( str, nullptr, 16);
                line++;

                _stream->ReadUntil('\n', str); // width
                polyline.width = str[0] ? (uint8_t)std::stoi( str) : 1;
                line++;

                _stream->ReadUntil('\n', str); // maxzoom
                polyline.maxzoom = str[0] ? (uint8_t)std::stoi( str) : max_zoom;
                line++;

                _stream->ReadUntil(':', str);

                if (strcmp(str, "bbox") != 0){
                    log_e("bbox error tag. Line %i : %s\n", line, str);
                    return false;
                }

                  polyline.bbox.min.x = read_int16(file);
                  polyline.bbox.min.y = read_int16(file);
                  polyline.bbox.max.x = read_int16(file);
                  polyline.bbox.max.y = read_int16(file);

                  // if( line > 4050){
                  //     log_e("polyline.bbox %i %i %i %i", polyline.bbox.min.x, polyline.bbox.min.y,polyline.bbox.max.x, polyline.bbox.max.y);
                  // }
                line++;

                polyline.points.clear();
                _stream->ReadUntil(':', str);
                if( strcmp( str, "coords") != 0){
                    log_d("coords tag. Line %i : %s\n", line, str);
                    return false;
                }

                ReadCoords(polyline.points);
                line++;
                result.polylines.push_back(polyline);
                total_points += polyline.points.size();
                count--;
            }
            return (count == 0);
        }

    private:
        StringReader* _stream;
};