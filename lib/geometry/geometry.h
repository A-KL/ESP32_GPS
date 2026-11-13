#pragma once

#include <math.h>
#include <vector>
#include <stdint.h>
#include <stdlib.h>

template<typename T>
struct Point {

    Point() : x(0), y(0)
    {};

    Point(T x, T y) : x(x), y(y) 
    {};

    Point(char *coords_pair) // char array like:  11.222,333.44
    {
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

    inline bool operator==(const Point& p) const
    {
        return (x == p.x && y == p.y); 
    };

    template<typename T2>
    inline Point<T2> To() const { 
        return Point<T2>(x, y);
    };

    T x;
    T y;
};

/// @brief Point in 16 bits projected coordinates (x,y) 
typedef Point<int16_t> Point16;

/// @brief Point in 32 bits projected coordinates (x,y) 
typedef Point<int32_t> Point32;

// @brief Bounding Box
struct BBox {
    BBox() 
    {};

    // @brief Bounding Box
    // @param min top left corner
    // @param max bottom right corner
    BBox(Point32 min, Point32 max)
    : min(min), max(max)
    {};

    BBox operator-(const Point32& point)
    { 
        return BBox(min-point, max-point);
    };

    inline bool containsPoint(const Point32& p)
    { 
        return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y; 
    }

    inline bool intersects(const BBox& b) const
    { 
        return !(b.min.x > max.x || b.max.x < min.x || b.min.y > max.y || b.max.y < min.y);
    }

    Point32 min;
    Point32 max;
};

// @brief Simple two points line 
struct Line {
    Line() 
    {};

    Line(Point16 p1, Point16 p2) : p1(p1), p2(p2) 
    {};

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
    uint8_t maxzoom;
};

// @brief Polygon feature 
struct Polygon {
    std::vector<Point16> points;
    BBox bbox;
    uint16_t color;
    uint8_t maxzoom;
};

struct viewport_t 
{
    viewport_t(const Point32& location, const uint16_t zoom, const uint16_t w, const uint16_t h) 
        : screen_width(w), screen_height(h), zoom_level(zoom)
    {
        setCenter(location);
    }

    void setCenter(const Point32& location) {
        center = location;
        bounding_box.min.x = location.x - screen_width  * zoom_level / 2;
        bounding_box.min.y = location.y - screen_height * zoom_level / 2;
        bounding_box.max.x = location.x + screen_width  * zoom_level / 2;
        bounding_box.max.y = location.y + screen_height * zoom_level / 2;
    }
   
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t zoom_level;

    Point32 center;
    BBox bounding_box;
};