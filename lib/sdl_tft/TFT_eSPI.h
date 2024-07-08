#pragma once

#ifndef ARDUINO

#include <SDL2/SDL.h>

class TFT_eSPI
{
public:
    TFT_eSPI();

    void setRender(SDL_Renderer* sdl);

    void init();

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
    uint16_t _textColor;

    int SDL_SetRenderDrawColor565(SDL_Renderer* render, uint16_t color565);

    void SDL_RenderText(SDL_Renderer* render, int32_t x0, int32_t y0, const char* msg);
};

#endif