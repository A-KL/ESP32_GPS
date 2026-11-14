#pragma once

#include "geometry.h"

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