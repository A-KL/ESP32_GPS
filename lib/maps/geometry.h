#pragma once

#include <vector>
#include <stdint.h>
#include <stdlib.h>

template<typename T>
struct Point 
{
    Point(T x = 0, T y = 0) : x(x), y(y) 
    {};

    Point(char *coords_pair) { // char array like:  11.222,333.44
        char *next;
        x = (T)round(strtod(coords_pair, &next));  // 1st coord // TODO: change by strtol and test
        y = (T)round(strtod(++next, NULL));        // 2nd coord
    }

    inline Point operator-(const Point& p) const {
        return Point(x-p.x, y-p.y);
    };

    inline Point operator+(const Point& p) const { 
        return Point(x+p.x, y+p.y);
    };

    inline bool operator==(const Point& p) const {
        return (x == p.x && y == p.y); 
    };

    template<typename T2>
    inline Point<T2> To() const { 
        return Point<T2>(x, y);
    };

    T x, y;
};

/// @brief Point in 16 bits projected coordinates (x,y) 
typedef Point<int16_t> Point16;

/// @brief Point in 32 bits projected coordinates (x,y) 
typedef Point<int32_t> Point32;

// @brief Bounding Box
struct BBox {
    BBox() 
    {}

    // @brief Bounding Box
    // @param min top left corner
    // @param max bottom right corner
    BBox(Point32 min, Point32 max) : min(min), max(max)
    {}

    BBox operator-(const Point32& point) const { 
        return BBox(min-point, max-point);
    };

    inline bool containsPoint(const Point32& p) const { 
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y; 
    }

    inline bool containsPoint(const Point16& p) const { 
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y; 
    }

    inline bool intersects(const BBox& b) const { 
        return !(b.min.x > max.x || b.max.x < min.x || b.min.y > max.y || b.max.y < min.y);
    }

    Point32 min, max;
};

// @brief Simple two points line 
struct Line {
    Line() 
    {}

    Line(Point16 p1, Point16 p2) : p1(p1), p2(p2) 
    {}

    Point16 p1;
    Point16 p2;
    uint16_t color;
    uint8_t width;
};

// @brief List of points, where line segments are drawn between consecutive points.
struct Polyline {
    std::vector<Point16> points;
    BBox bbox;
    uint16_t color;
    uint8_t width;
    uint8_t max_zoom;
};

// @brief Polygon feature 
struct Polygon {
    std::vector<Point16> points;
    BBox bbox;
    uint16_t color;
    uint8_t max_zoom;
};

/// @brief Map square area of aprox 4096 meters side. Correspond to one single map file.
struct MapBlock {
    Point32 offset;
    // BBox bbox;
    bool inView = false;
    std::vector<Polyline> polylines;
    std::vector<Polygon> polygons;
};

/// @brief MapBlocks stored in memory
struct MemCache {
    std::vector<MapBlock*> blocks;
    // uint32_t total_polylines = 0;
    // uint32_t total_polygons = 0;
    // uint32_t total_inview_polylines = 0;
    // uint32_t total_inview_polygons = 0;
};