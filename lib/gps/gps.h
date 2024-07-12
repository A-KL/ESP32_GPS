#pragma once

#include <geometry.h>

#ifndef M_PI
#define M_PI    3.14159265358979323846264338327950288   /**< pi */
#endif

#define DEG2RAD(a)   ((a) / (180 / M_PI))
#define RAD2DEG(a)   ((a) * (180 / M_PI))
#define EARTH_RADIUS 6378137

inline double lat2y(double lat) { return log(tan( DEG2RAD(lat) / 2 + M_PI/4 )) * EARTH_RADIUS; }
inline double lon2x(double lon) { return          DEG2RAD(lon)                 * EARTH_RADIUS; }

/// @brief Point in geografic (lat,lon) coordinates and other gps data
struct Coord {
    Coord(): Coord(0, 0)
    {}
    Coord(double _lat, double _lng): lat(_lat), lng(_lng)
    {}
    Point32 getPoint32() {
        return Point32(lon2x(lng), lat2y(lat));
    }
    double lat = 0;
    double lng = 0;
    int16_t altitude = 0;
    int16_t direction = 0;
    int16_t satellites = 0;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;
    bool isValid = false;
    bool isUpdated = false;
};

void gpsInit();
void gpsGetPosition(Coord& coord);