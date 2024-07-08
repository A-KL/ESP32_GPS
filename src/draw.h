#pragma once 

struct MemCache;
struct ViewPort;
struct Coord;

// class MapViewport
// {
//     public:

//     private:
//         inline int16_t toScreenCoord(const int32_t pxy, const int32_t screen_center_xy, int zoom_level) // work with primitives for performance
//         {
//             return round((double)(pxy - screen_center_xy) / zoom_level) + (double)SCREEN_WIDTH / 2.0;
//         }
// }

void tft_header(Coord& pos);

void tft_footer(const char* msg);

void tft_msg(const char* msg);

void header_msg(const char* msg);

void draw(ViewPort& viewPort, MemCache& memCache, int zoom_level, int mode);