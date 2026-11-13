#include <vector>
#include <string>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "TFT_eSPI.h"

#define RED(r)   (uint8_t)(round(r >> 11) / 31.0 * 255)
#define GREEN(g) (uint8_t)(round((g >> 5) & 0b111111) / 63.0 * 255)
#define BLUE(b)  (uint8_t)(round(b & 0b11111) / 31.0 * 255)


TFT_eSPI::TFT_eSPI() 
    : _sdl(NULL), _cursorX(0), _cursorY(0), _textColor(0), _w(TFT_WIDTH), _h(TFT_HEIGHT)
{};

TFT_eSPI::TFT_eSPI(uint16_t w, uint16_t h) : _sdl(NULL), _cursorX(0), _cursorY(0), _textColor(0), _w(w), _h(h)
{};

int16_t TFT_eSPI::width()
{
    return _w;
}

int16_t TFT_eSPI::height()
{
    return _h;
}

void TFT_eSPI::setRender(SDL_Renderer* sdl)
{
    _sdl = sdl;
}

void TFT_eSPI::init()
{
    
}

void TFT_eSPI::flush()
{
    SDL_RenderPresent(_sdl);
}

void TFT_eSPI::setRotation(int rotation)
{

}

void TFT_eSPI::invertDisplay(bool invert)
{

}

void TFT_eSPI::fillScreen(uint16_t color)
{
    SDL_SetRenderDrawColor565(_sdl, color);
    SDL_RenderClear(_sdl);
}

void TFT_eSPI::setTextColor(uint16_t color)
{
    _textColor = color;
}

void TFT_eSPI::setCursor(int x, int y, int font)
{
    _cursorX = x;
    _cursorY = y;
}

void TFT_eSPI::println(const char* text)
{
    SDL_RenderText(_sdl, _cursorX, _cursorY, text);
}

void TFT_eSPI::print(const char* text)
{
    SDL_RenderText(_sdl, _cursorX, _cursorY, text);
}

void TFT_eSPI::printf(const char* format, ...)
{
    char text[50];
    va_list args;
    va_start(args, format);
    vsnprintf(text, 50, format, args);
   // vprintf(format, args);
    va_end(args);  
    SDL_RenderText(_sdl, _cursorX, _cursorY, text);
}

void TFT_eSPI::print(const char* text, int position)
{
    SDL_RenderText(_sdl, _cursorX, _cursorY, text);
}

void TFT_eSPI::print(int value)
{
    SDL_RenderText(_sdl, 0, 0, std::to_string(value).c_str());
}

void TFT_eSPI::print(int value, int position)
{
    SDL_RenderText(_sdl, 0, 0, std::to_string(value).c_str());
}

void TFT_eSPI::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color)
{
    SDL_SetRenderDrawColor565(_sdl, color);

    SDL_Rect rect { x, y+r, w, h-2*r };
    SDL_RenderFillRect(_sdl, &rect);

    fillCircle(x+r, y+r, r, color, 180, 270);
    fillCircle(x+w-r, y+r, r, color, 270, 360);
    fillCircle(x+r, y+h-r, r, color, 90, 180);
    fillCircle(x+w-r, y+h-r, r, color, 0, 90);

    SDL_Rect rect2 { x+r, y, w-2*r, h };
    SDL_RenderFillRect(_sdl, &rect2);
}

void TFT_eSPI::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
    SDL_SetRenderDrawColor565(_sdl, color);

    SDL_Rect rect { x, y, w, h };
    SDL_RenderFillRect(_sdl, &rect);
}

void TFT_eSPI::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
    SDL_SetRenderDrawColor565(_sdl, color);
    SDL_RenderDrawLine(_sdl, x0, y0, x1, y1);
}

void TFT_eSPI::drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color)
{
    auto d = sqrtf((bx - ax)*(bx - ax) + (by - ay)*(by - ay));
    auto y_shift = wd * (bx - ax) / (d * 2.0f);
    auto x_shift = - wd * (by - ay) / (d * 2.0f);

    const SDL_Color sdl_color 
    { 
        RED(fg_color),
        GREEN(fg_color),
        BLUE(fg_color),
    };

    const std::vector<SDL_Vertex> verts =
    {
        { SDL_FPoint{ ax - x_shift, ay - y_shift }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ ax + x_shift, ay + y_shift }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ bx + x_shift, by + y_shift }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ bx - x_shift, by - y_shift }, sdl_color, SDL_FPoint{ 0 }, },
    };

    int indexes[] = { 0,1,2, 0,2,3 };

    SDL_RenderGeometry(_sdl, nullptr, verts.data(), verts.size(), indexes, sizeof(indexes)/sizeof(int));
}

void TFT_eSPI::fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
    const SDL_Color sdl_color
    { 
        RED(color),
        GREEN(color),
        BLUE(color)
    };

    const std::vector<SDL_Vertex> verts =
    {
        { SDL_FPoint{ (float)x0, (float)y0 }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ (float)x1, (float)y1 }, sdl_color, SDL_FPoint{ 0 }, },
        { SDL_FPoint{ (float)x2, (float)y2 }, sdl_color, SDL_FPoint{ 0 }, },
    };

    auto res = SDL_RenderGeometry(_sdl, nullptr, verts.data(), verts.size(), nullptr, 0);
}

void TFT_eSPI::fillCircle(int32_t x, int32_t y, int32_t radius, uint16_t color, int32_t startAngle, int32_t endAngle)
{
    const int64_t sweep = static_cast<int64_t>(endAngle) - startAngle;
    const bool fullCircle = sweep >= 360 || sweep <= -360;
    const auto normalizeAngle = [](int32_t angle) {
        const int32_t normalized = angle % 360;
        return normalized < 0 ? normalized + 360 : normalized;
    };
    const int32_t start = normalizeAngle(startAngle);
    const int32_t end = normalizeAngle(endAngle);

    if (!fullCircle && start == end) return;

    SDL_SetRenderDrawColor565(_sdl, color);

    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                bool inSweep = fullCircle || (dx == 0 && dy == 0);
                if (!inSweep) {
                    double angle = std::atan2(static_cast<double>(dy), static_cast<double>(dx)) * 180.0 / 3.14159265358979323846;
                    if (angle < 0) angle += 360.0;
                    inSweep = start < end
                        ? angle >= start && angle <= end
                        : angle >= start || angle <= end;
                }

                if (inSweep) SDL_RenderDrawPoint(_sdl, x + dx, y + dy);
            }
        }
    }
}

inline int TFT_eSPI::SDL_SetRenderDrawColor565(SDL_Renderer* render, uint16_t color)
{
    return SDL_SetRenderDrawColor(
        render,
        RED(color),
        GREEN(color),
        BLUE(color),
        SDL_ALPHA_OPAQUE);
}

void TFT_eSPI::SDL_RenderText(SDL_Renderer* render, int32_t x0, int32_t y0, const char* msg)
{
    SDL_Color Black = {0, 0, 0};

    TTF_Font* font = TTF_OpenFont(FONT_NAME, FONT_SIZE);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, msg, Black);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(render, textSurface);

    SDL_Rect textRect { x0, y0, 0, 0 };

    TTF_SizeText(font, msg, &textRect.w, &textRect.h);

    SDL_RenderCopy(render, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}