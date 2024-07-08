
#include <colors.h>
#include <geometry.h>
#include <gps.h>
#include <files.h>
#include <maps.h>
#include <canvas.h>
#include "../lib/conf.h"
#include "draw.h"

#ifdef ARDUINO
    #include "arduino_app.h"
#else
    #include <logs.h>
    #include "sdl_app.h"
#endif

MemCache memCache;
Coord coord;
std::vector<Coord> samples;

int zoom_level = PIXEL_SIZE_DEF; // zoom_level = 1 correspond aprox to 1 meter / pixel
int mode = DEVMODE_NAV;

double prev_lat = 500;
double prev_lng = 500;

Point32 map_center(INIT_POS);
ViewPort viewPort(map_center, zoom_level, SCREEN_WIDTH, SCREEN_HEIGHT);

IFileSystem* fileSystem = get_file_system(MAPS_LOCATION);

void Setup()
{
    tft_init();

    tft_footer_msg(std::to_string(zoom_level).c_str());

    if (!init_file_system()) {
        tft_println("Error: SD Card Mount Failed!");
        while(true);
    }
    log_i("SRat value: %d\n", 123);

    tft_println("Reading map...");

    Point32 map_center(INIT_POS);
    // TODO: keep and show last position

    viewPort.setCenter(map_center);

    auto result = get_map_blocks(fileSystem, viewPort.bbox, memCache);
    while (!result);

    draw(viewPort, memCache, zoom_level, mode);

    tft_msg("Waiting for satellites...");
}

bool Loop()
{
    Point32 p = viewPort.center;
    bool moved = false;

    if (mode == DEVMODE_NAV) {
        Coord coord;
        gpsGetPosition(coord);

        if (coord.isValid && 
            abs(coord.lat-prev_lat) > 0.00005 &&
            abs(coord.lng-prev_lng) > 0.00005) {
                p = coord.getPoint32();
                prev_lat = coord.lat;
                prev_lng = coord.lng;
                moved = true;
            }   
    }

    if (ReadInput(SELECT_BUTTON)) {
        mode += 1;
        if( mode > DEVMODE_ZOOM){ 
            mode = DEVMODE_NAV;
            moved = true; // recenter
        }
        tft_header(coord, mode);
       //delay(200); // button debouncing
    }

    if (mode == DEVMODE_MOVE) {
        if( ReadInput( UP_BUTTON))    { p.y += 40*zoom_level; moved = true; }
        if( ReadInput( DOWN_BUTTON))  { p.y -= 40*zoom_level; moved = true; }
        if( ReadInput( LEFT_BUTTON))  { p.x -= 40*zoom_level; moved = true; }
        if( ReadInput( RIGHT_BUTTON)) { p.x += 40*zoom_level; moved = true; }
    }

    if (mode == DEVMODE_ZOOM) {
        if( ReadInput( UP_BUTTON))    { zoom_level += 1; moved = true; }
        if( ReadInput( DOWN_BUTTON))  { zoom_level -= 1; moved = true; }
        if( zoom_level < 1){ zoom_level = 1; moved = false; } 
        if( zoom_level > MAX_ZOOM){ zoom_level = MAX_ZOOM; moved = false; }
    }

    if (moved) {
        viewPort.setCenter(p);
        auto res = get_map_blocks(fileSystem, viewPort.bbox, memCache);
        draw(viewPort, memCache, zoom_level, mode);
        tft_header(coord, mode);
        tft_footer_msg(std::to_string(zoom_level).c_str());
        delay(10);
    }

    return moved;
}