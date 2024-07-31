#pragma once

#ifndef ARDUINO

#include <SDL2/SDL.h>

class TFT_eSPI
{
public:
    TFT_eSPI();

    TFT_eSPI(uint16_t w, uint16_t h);

    int16_t width();

    int16_t height();

    void setRender(SDL_Renderer* sdl);

    void init();

    void flush();

	void setRotation(int rotation);
	
	void invertDisplay(bool invert);

	void fillScreen(uint16_t color);

	void setTextColor(uint16_t color);

	void setCursor(int x, int y, int font);

	void println(const char* text);

	void print(const char* text);

    void print(const char* text, int position);

    void print(int value);

    void print(int value, int position);

    void printf(const char* format, ...);

	void fillRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color);

    void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color);

    void drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color);

    void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

    void fillCircle(int32_t x0, int32_t y0, int32_t radius, uint16_t color);

private:
    SDL_Renderer* _sdl;
    uint16_t _cursorX, _cursorY;
    uint16_t _w, _h;
    uint16_t _textColor;

    int SDL_SetRenderDrawColor565(SDL_Renderer* render, uint16_t color565);

    void SDL_RenderText(SDL_Renderer* render, int32_t x0, int32_t y0, const char* msg);
};

class TFT_eSprite
{
public:
    TFT_eSprite(TFT_eSPI* tft)
    : _tft(tft), _depth(8), _w(10), _h(10), _bgcolor(0xFF)
    {
    }

    void setColorDepth(int depth) {
        _depth = depth;
    }

    void createSprite(int w, int h) {
        _w = w;
        _h = h;
    }

    void fillSprite(uint16_t color) {
        _bgcolor = color;
    }
    
private:
    TFT_eSPI* _tft;
    int _depth, _w, _h;
    uint16_t _bgcolor;
};

#endif