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
    return color565_scale(red5);
}

inline uint8_t color565_green(uint16_t color565)
{
    auto green6 = (color565 >> 5) & 0b111111;
    return color565_scale(green6);
}

 inline uint8_t color565_blue(uint16_t color565)
{
    auto blue5 = color565 & 0b11111;
    return color565_scale(blue5);
}

inline int SDL_SetRenderDrawColor(SDL_Renderer* render, uint16_t color565)
{
    return SDL_SetRenderDrawColor(
        render, 
        color565_red(color565),
        color565_green(color565),
        color565_blue(color565),
        
        //  RED(color565),
        //  GREEN(color565),
        //  BLUE(color565),
        0); //SDL_ALPHA_OPAQUE
}

void tft_init()
{ }

void tft_println(const char* text)
{
// TODO
}

void tft_msg(const char* msg)
{
// TODO
}

void tft_header(const Coord& pos, const int mode)
{
    SDL_Rect rect { 0, 0, TFT_WIDTH, 25 };

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
    SDL_Rect rect { 0, 0, TFT_WIDTH, 25 };

    SDL_SetRenderDrawColor(_sdl, YELLOWCLEAR);
    SDL_RenderFillRect(_sdl, &rect);

    // tft.fillRect(0, 0, 240, 25, YELLOWCLEAR);
    // tft.setCursor(5,5,2);
    // tft.print(msg);
}

void tft_footer_msg(const char* msg)
{
    SDL_Rect rect { 0, 300, TFT_WIDTH, TFT_HEIGHT};

    SDL_SetRenderDrawColor(_sdl, CYAN);
    SDL_RenderFillRect(_sdl, &rect);

    // tft.fillRect(0, 300, 240, 320, CYAN);
    // tft.setCursor(5,305,2);
    // tft.println(msg);
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
    auto d = std::sqrtf((bx - ax)*(bx - ax) + (by - ay)*(by - ay));
    auto y_shift = wd * (bx - ax) / (d * 2.0f);
    auto x_shift = - wd * (by - ay) / (d * 2.0f);

    const SDL_Color sdl_color 
    { 
        color565_red(fg_color),
        color565_green(fg_color),
        color565_blue(fg_color),
     };

    const std::vector<SDL_Vertex> verts =
    {
        { SDL_FPoint{ ax - x_shift, ay - y_shift }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ ax + x_shift, ay + y_shift }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ bx + x_shift, by + y_shift }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ bx - x_shift, by - y_shift }, sdl_color, SDL_FPoint{ 0 }, },
    };

    int indexs[] = { 0,1,2, 0,2,3 };

    auto res = SDL_RenderGeometry(_sdl, nullptr, verts.data(), verts.size(), indexs, sizeof(indexs)/sizeof(int));
}

void tft_fill_triangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color565)
{
    const SDL_Color sdl_color
    { 
        color565_red(color565),
        color565_green(color565),
        color565_blue(color565),
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