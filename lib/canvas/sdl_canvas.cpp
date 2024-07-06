#include <colors.h>
#include <gps.h>

#include "canvas.h"
#include "../conf.h"

#ifndef ARDUINO

#include <logs.h>
#include <SDL2/SDL.h>

extern SDL_Renderer* _sdl;



#define RED(r)   (uint8_t)(round(r >> 11) / 31 * 255)
#define GREEN(g) (uint8_t)(round((g >> 5) & 0b111111) / 31 * 255)
#define BLUE(b)  (uint8_t)(round(b & 0b11111) / 31 * 255)

inline uint8_t color565_scale(uint16_t color)
{
    return round(color / 31 * 255);
}

inline uint8_t color565_red(uint16_t color565)
{
    auto red5 = color565 >> 11;
    // auto green6 = (color565 >> 5) & 0b111111;
    // auto blue5 = color565 & 0b11111;

    return color565_scale(red5);
}

inline uint8_t color565_green(uint16_t color565)
{
    auto red5 = color565 >> 11;
    auto green6 = (color565 >> 5) & 0b111111;
    auto blue5 = color565 & 0b11111;

    return color565_scale(green6);
}

inline uint8_t color565_blue(uint16_t color565)
{
    // auto red5 = color565 >> 11;
    // auto green6 = (color565 >> 5) & 0b111111;
    auto blue5 = color565 & 0b11111;

    return color565_scale(blue5);
}

inline int SDL_SetRenderDrawColor(SDL_Renderer* render, uint16_t color565)
{
    log_d("R: %d, G: %d, B: %d", color565_red(YELLOWCLEAR), color565_green(YELLOWCLEAR), color565_blue(YELLOWCLEAR));
    return SDL_SetRenderDrawColor(
        render, 
        RED(color565),
        GREEN(color565),
        BLUE(color565),
        0); //SDL_ALPHA_OPAQUE
}

void tft_init()
{ }

void tft_println(const char* text)
{
// TODO
}

void tft_header(const Coord& pos, const int mode)
{
    SDL_Rect rect { 0, 0, 240, 25 };

    SDL_SetRenderDrawColor(_sdl, YELLOWCLEAR);
    SDL_RenderFillRect(_sdl, &rect);

    // tft.fillRect(0, 0, 240, 25, YELLOWCLEAR);
    // tft.setCursor(5,5,2);
    // tft.print(pos.lng, 4);
    // tft.print(" "); tft.print(pos.lat, 4);
    // tft.print(" Sats: "); tft.print(pos.satellites);
    // tft.print(" M: "); tft.print(mode);
}

void tft_header_msg(const char* msg)
{
    SDL_Rect rect { 0, 0, 240, 25 };

    SDL_SetRenderDrawColor(_sdl, YELLOWCLEAR);
    SDL_RenderFillRect(_sdl, &rect);

    // tft.fillRect(0, 0, 240, 25, YELLOWCLEAR);
    // tft.setCursor(5,5,2);
    // tft.print(msg);
}

void tft_footer(const char* msg)
{
    SDL_Rect rect { 0, 300, 240, 320};

    SDL_SetRenderDrawColor(_sdl, CYAN);
    SDL_RenderFillRect(_sdl, &rect);

    // tft.fillRect(0, 300, 240, 320, CYAN);
    // tft.setCursor(5,305,2);
    // tft.println(msg);
}


void tft_msg(const char* msg)
{
// TODO
}

void tft_fill_screen()
{
    SDL_SetRenderDrawColor(_sdl, BACKGROUND_COLOR);
	SDL_RenderClear(_sdl);
}

void tft_draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    SDL_SetRenderDrawColor(_sdl, color);
    auto res = SDL_RenderDrawLine(_sdl, x0, y0, x1, y1);
}

void tft_draw_wide_line(float ax, float ay, float bx, float by, float wd, uint32_t fg_color, uint32_t bg_color)
{
    SDL_SetRenderDrawColor(_sdl, fg_color);
    auto res = SDL_RenderDrawLine(_sdl, ax, ay, bx, by);
    // TODO
}

void tft_fill_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
{
    SDL_SetRenderDrawColor(_sdl, color);

    const SDL_Color sdl_color 
    { 
        RED(color),
        GREEN(color),
        BLUE(color),
     };

    const std::vector<SDL_Vertex> verts =
    {
        { SDL_FPoint{ (float)x0, (float)y0 }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ (float)x1, (float)y1 }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ (float)x2, (float)y2 }, sdl_color, SDL_FPoint{ 0 }, },
    };

   auto res = SDL_RenderGeometry(_sdl, nullptr, verts.data(), verts.size(), nullptr, 0);
}

#endif