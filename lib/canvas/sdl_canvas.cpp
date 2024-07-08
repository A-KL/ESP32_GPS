#include <colors.h>
#include <gps.h>

#include "canvas.h"
#include "../conf.h"

#ifndef ARDUINO

#include <cstdio>
#include <logs.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

extern SDL_Renderer* _sdl;

#define RED(r)   (uint8_t)(round(r >> 11) / 31.0 * 255)
#define GREEN(g) (uint8_t)(round((g >> 5) & 0b111111) / 63.0 * 255)
#define BLUE(b)  (uint8_t)(round(b & 0b11111) / 31.0 * 255)

inline uint8_t color565_red(uint16_t color565)
{
    auto red5 = color565 >> 11;
    return round(red5 / 31.0 * 255);
}

inline uint8_t color565_green(uint16_t color565)
{
    auto green6 = (color565 >> 5) & 0b111111;
    return round(green6 / 63.0 * 255);
}

 inline uint8_t color565_blue(uint16_t color565)
{
    auto blue5 = color565 & 0b11111;
    return round(blue5 / 31.0 * 255);
}

inline int SDL_SetRenderDrawColor(SDL_Renderer* render, uint16_t color565)
{
    return SDL_SetRenderDrawColor(
        render,
        // color565_red(color565),
        // color565_green(color565),
        // color565_blue(color565),
         RED(color565),
         GREEN(color565),
         BLUE(color565),
        SDL_ALPHA_OPAQUE);
}

void SDL_RenderText(int32_t x0, int32_t y0, const char* msg)
{
    SDL_Color White = {0, 0, 0};

    TTF_Font* font = TTF_OpenFont(FONT_NAME, FONT_SIZE);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, msg, White);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_sdl, textSurface);

    SDL_Rect textRect { x0, y0, 0, 0 };

    TTF_SizeText(font, msg, &textRect.w, &textRect.h);

    SDL_RenderCopy(_sdl, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

void tft_init()
{ }

void tft_println(const char* msg)
{
    SDL_Rect rect { 0, 0, SCREEN_WIDTH, 25 };

    SDL_SetRenderDrawColor(_sdl, YELLOWCLEAR);
    SDL_RenderFillRect(_sdl, &rect);

    SDL_RenderText(2, 2, msg);
}

void tft_msg(const char* msg)
{
    SDL_Rect rect { 0, 0, SCREEN_WIDTH, 25 };

    SDL_SetRenderDrawColor(_sdl, CYAN);
    SDL_RenderFillRect(_sdl, &rect);

    SDL_RenderText(2, 2, msg);
}

void tft_header_msg(const char* msg)
{
    SDL_Rect rect { 0, 0, SCREEN_WIDTH, 25 };

    SDL_SetRenderDrawColor(_sdl, YELLOWCLEAR);
    SDL_RenderFillRect(_sdl, &rect);

    SDL_RenderText(2, 2, msg);
}

void tft_header(const Coord& pos, const int mode)
{
    char text[50];
    snprintf(text, 50, "%f %f Sats: %d M: %d", pos.lng, pos.lat, pos.satellites, mode);

    tft_header_msg(text);
}

void tft_footer_msg(const char* msg)
{
    SDL_Rect rect { 0, SCREEN_HEIGHT - 20, SCREEN_WIDTH, SCREEN_HEIGHT};

    SDL_SetRenderDrawColor(_sdl, CYAN);
    SDL_RenderFillRect(_sdl, &rect);

    SDL_RenderText(2, SCREEN_HEIGHT + 2, msg);
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
    auto d = sqrtf((bx - ax)*(bx - ax) + (by - ay)*(by - ay));
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

    int indexes[] = { 0,1,2, 0,2,3 };

    auto res = SDL_RenderGeometry(_sdl, nullptr, verts.data(), verts.size(), indexes, sizeof(indexes)/sizeof(int));
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

void tft_fill_circle(int32_t x0, int32_t y0, int32_t radius,  uint32_t color565)
{
    SDL_SetRenderDrawColor(_sdl, color565);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(_sdl, x0 + dx, y0 + dy);
            }
        }
    }
}

#endif