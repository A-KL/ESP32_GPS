#include <math.h>
#include <stdint.h>

#include "../conf.h"

#include "gps.h"

#ifdef ARDUINO

#include <Arduino.h>
#include <TinyGPS++.h>

TinyGPSPlus gps;
HardwareSerial SerialGPS(1);

void gpsInit()
{
    Serial.begin(115200);
    #ifndef ARDUINO_uPesy_WROVER
       // SerialGPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    #endif
}

/// @brief Get the current position from the GPS chip
/// @param serialGPS handler
/// @return projected coordinates in meters
void getPosition(HardwareSerial& serialGPS, Coord& coord)
{
    while (serialGPS.available() > 0)
    {
        if (gps.encode(serialGPS.read()))
        {
            coord.lat = gps.location.lat();
            coord.lng = gps.location.lng();

            coord.altitude = static_cast<int16_t>(gps.altitude.meters());
            coord.direction = static_cast<int16_t>(gps.course.deg()); // degrees
            coord.satellites = static_cast<int16_t>(gps.satellites.value());
            
            coord.isValid = gps.location.isValid();
            coord.isUpdated = gps.location.isUpdated();

            coord.hour = gps.time.hour();
            coord.minute = gps.time.minute();
            coord.second = gps.time.second();
        }
    }
}

void gpsGetPosition(Coord& coord)
{
    coord.lat = DEFAULT_LAT;
    coord.lng = DEFAULT_LON;

    coord.altitude = static_cast<int16_t>(10);
    coord.direction = static_cast<int16_t>(15); // degrees
    coord.satellites = static_cast<int16_t>(4);
    
    coord.isValid = true;
    coord.isUpdated = true;

    coord.hour = 12;
    coord.minute = 33;
    coord.second = 12;
    //getPosition(SerialGPS, coord);
}

// Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
// Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
// Serial.print("ALT=");  Serial.println(gps.altitude.meters());
// Serial.print("day ");  Serial.println(gps.date.day());
// Serial.print("year ");  Serial.println(gps.date.year());
// Serial.print("hour ");  Serial.println(gps.time.hour());
// Serial.print("minute ");  Serial.println(gps.time.minute());
// Serial.print("satellites ");  Serial.println(gps.satellites.value());

#else

#include <iostream>
#include <fstream>
#include <iomanip>
//#include <nmeaparse/nmea.h>

using namespace std;
// using namespace nmea;

// NMEAParser parser;
// GPSService gps(parser);

void gpsInit()
{}

void gpsGetPosition(Coord& coord)
{
    coord.lat = DEFAULT_LAT;
    coord.lng = DEFAULT_LON;

    coord.altitude = static_cast<int16_t>(10);
    coord.direction = static_cast<int16_t>(15); // degrees
    coord.satellites = static_cast<int16_t>(4);
    
    coord.isValid = true;
    coord.isUpdated = true;

    coord.hour = 12;
    coord.minute = 33;
    coord.second = 12;
}

#endif

int coord_to_point(const Coord& coord, Point32& result) {
    result.x = lon2x(coord.lng);
    result.y = lat2y(coord.lat);
 }

 int point_to_coord(const Point32& point, Coord& result) {
    result.lng = RAD2DEG(point.x / (float)EARTH_RADIUS);
    result.lat = RAD2DEG(point.y / (float)EARTH_RADIUS); //Fix this
 }