
#include <colors.h>
#include <geometry.h>
#include <gps.h>
#include <files.h>
#include <maps.h>
#include "../lib/conf.h"
#include "draw.h"

#ifdef ARDUINO
    #include "arduino_app.h"
#else
    #include "sdl_app.h"
    #include <_Arduino.h>
    #include <logs.h>
#endif

TFT_eSPI tft;
TFT_eSprite img = TFT_eSprite(&tft);

MemCache memCache;
std::vector<Coord> samples;

double prev_lat = DEFAULT_LAT;
double prev_lng = DEFAULT_LON;
int zoom_level = PIXEL_SIZE_DEF; // zoom_level = 1 correspond aprox to 1 meter / pixel
int mode = DEVMODE_NAV;//DEVMODE_MOVE; //DEVMODE_NAV
bool moved = true;

Coord map_center_coord(DEFAULT_LAT, DEFAULT_LON);
Point32 map_center_point = map_center_coord.getPoint32();
ViewPort viewPort(map_center_point, zoom_level, tft.width(), tft.height());
IFileSystem* fileSystem = get_file_system(MAPS_LOCATION);

void printFreeMem() {
    log_i("FreeHeap: %i\n", esp_get_free_heap_size());
    log_i("Heap minimum_free_heap_size: %i\n", esp_get_minimum_free_heap_size());
    log_i("RAM esp_spiram_get_size: %i\n", esp_spiram_get_size());
    // log_i("Heap largest_free_block: %i\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    // log_i("Task watermark: %i\n", uxTaskGetStackHighWaterMark(NULL));
}

void gpioInit() {
    // pinMode( UP_BUTTON, INPUT_PULLUP);
    // pinMode( DOWN_BUTTON, INPUT_PULLUP);
    // pinMode( LEFT_BUTTON, INPUT_PULLUP);
    // pinMode( RIGHT_BUTTON, INPUT_PULLUP);
    // pinMode( SELECT_BUTTON, INPUT_PULLUP);
    // pinMode( TFT_OFF_BUTTON, INPUT_PULLUP);
    //pinMode( MENU_BUTTON, INPUT_PULLUP);
    pinMode( TFT_BLK_PIN, OUTPUT);
    // pinMode( GPS_CE, OUTPUT);
}

void tftOn() {
    analogWrite(TFT_BLK_PIN, 80);
}

void tftfOff() {
    digitalWrite(TFT_BLK_PIN, LOW); // switch off display
}

void setup()
{
    pinMode(PWR_EN_PIN, OUTPUT);
    digitalWrite(PWR_EN_PIN, HIGH);

    pinMode(PWR_ON_PIN, OUTPUT);
    digitalWrite(PWR_ON_PIN, HIGH);

    gpioInit();
    tftfOff();

    //printFreeMem();
    gpsInit();

    //digitalWrite(SD_CS_PIN, HIGH); // SD card chips select
    digitalWrite(TFT_CS, HIGH); // TFT chip select

    tft.init();
    tft.setRotation(TFT_ROTATE);
    tft.fillScreen(CYAN);
    tft.setTextColor(BLACK);
    tft.setCursor(5,5,4);
    tft.println("Initializing...");
    tftOn();

    img.setColorDepth(8);
    img.createSprite(tft.width(), tft.height());
    img.fillSprite(BACKGROUND_COLOR);

   // printFreeMem();

    while (!init_file_system())
    {
        tft.println("Error: SD Card Mount Failed!");
        log_e("Error: SD Card Mount Failed!");
        delay(1000);
    }

    tft.println("Reading map...");
    viewPort.setCenter(map_center_point);
    auto result = get_map_blocks(fileSystem, viewPort.bbox, memCache);

    if (!result) {
        tft.println("Error: Unable to read map!");
        stop();
    }
    
    draw(tft, viewPort, memCache, zoom_level, mode);

    tft_msg(tft, "Waiting for satellites...");

    // stats(viewPort, mmap);
    // printFreeMem();
    // sleepInit();

    tft.flush();
}

void loop()
{   
    Coord coord;
    Point32 p = viewPort.center;
    //sleepLoop();

    if (mode == DEVMODE_NAV) {
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

    if (digitalRead(SELECT_BUTTON)) {
        mode += 1;
        if( mode > DEVMODE_ZOOM){ 
            mode = DEVMODE_NAV;
            moved = true; // recenter
        }
        tft_header(tft, coord, mode);
        delay(200); // button debouncing
    }

    if (mode == DEVMODE_MOVE) {
        if( digitalRead( UP_BUTTON))    { p.y += 40*zoom_level; moved = true; }
        if( digitalRead( DOWN_BUTTON))  { p.y -= 40*zoom_level; moved = true; }
        if( digitalRead( LEFT_BUTTON))  { p.x -= 40*zoom_level; moved = true; }
        if( digitalRead( RIGHT_BUTTON)) { p.x += 40*zoom_level; moved = true; }
        // point_to_coord(p, coord);
    }

    if (mode == DEVMODE_ZOOM) {
        if( digitalRead( UP_BUTTON))    { zoom_level += 1; moved = true; }
        if( digitalRead( DOWN_BUTTON))  { zoom_level -= 1; moved = true; }
        if( zoom_level < 1){ zoom_level = 1; moved = false; } 
        if( zoom_level > MAX_ZOOM){ zoom_level = MAX_ZOOM; moved = false; }
    }

    if (moved) {
        viewPort.setCenter(p);
        auto res = get_map_blocks(fileSystem, viewPort.bbox, memCache);
        draw(tft, viewPort, memCache, zoom_level, mode);
        tft_header(tft, coord, mode);
        tft_footer(tft, zoom_level);
        delay(10);
    }

    if (moved) {
        tft.flush();
        moved = false;
    }
}