#pragma once 

struct MemCache;
struct ViewPort;
struct Coord;
class TFT_eSPI;

void tft_header(TFT_eSPI& tft, Coord& pos, int mode);

void tft_footer(TFT_eSPI& tft, int zoom);

void tft_msg(TFT_eSPI& tft, const char* msg);

void header_msg(TFT_eSPI& tft, const char* msg);

void draw(TFT_eSPI& tft, ViewPort& viewPort, MemCache& memCache, int zoom_level, int mode);