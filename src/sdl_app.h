#pragma once

#define SDL_MAIN_HANDLED

#include <chrono>
#include <iostream>
#include <stdio.h>

#include <TFT_eSPI.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

SDL_Renderer* _sdl;
SDL_Event event;

bool mouse_down = false;
int mouse_x = 0, mouse_y = 0;

extern TFT_eSPI tft;

void setup();

void loop();

#define CUSTOM_DELAY
void delay(uint32_t msec)
{
    SDL_Delay(msec);
}

#define CUSTOM_DIGITAL_READ
int digitalRead(int pin)
{
	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
		SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
		mouse_down = true;
		break;
	case SDL_MOUSEBUTTONUP:
		mouse_x = 0, mouse_y = 0;
		mouse_down = false;
		break;
	case SDL_MOUSEMOTION:
		if (mouse_down)
		{
			int local_x = 0, local_y = 0;
			SDL_GetGlobalMouseState(&local_x, &local_y);
			auto dx = local_x - mouse_x;
			auto dy = local_y - mouse_y;
			//std::cout << dx << "  " << dy << std::endl;
			switch (pin)
			{
				case UP_BUTTON:
					return dy < 0;
			
				case DOWN_BUTTON:
					return dy > 0;

				case LEFT_BUTTON:
					return dx < 0;

				case RIGHT_BUTTON:
					return dx > 0;
			}
		}
		break;
	default:
		break;
	}

	const Uint8* state = SDL_GetKeyboardState(nullptr);

	switch (pin)
	{
		case SELECT_BUTTON:
			return state[SDL_SCANCODE_S];

		case UP_BUTTON:
			return state[SDL_SCANCODE_UP];
	
		case DOWN_BUTTON:
			return state[SDL_SCANCODE_DOWN];

		case LEFT_BUTTON:
			return state[SDL_SCANCODE_LEFT];

		case RIGHT_BUTTON:
			return state[SDL_SCANCODE_RIGHT];

		default:
			return false;
	}
}

void stop()
{}

void serialInit()
{}

void sleepInit()
{}

void sleepLoop()
{}

int main()
{
    auto init_sdl = SDL_Init(SDL_INIT_VIDEO);
	auto init_tff = TTF_Init();

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	if (init_sdl == -1) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (init_tff == -1) {
		fprintf(stderr, "Unable to initialize SDL_TTF: %s\n", TTF_GetError());
		return 1;
	}

    auto window = SDL_CreateWindow(
		"SDL Maps",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, 
        SCREEN_HEIGHT, 
		SDL_WINDOW_RESIZABLE ); //| SDL_WINDOW_METAL

	if (window == NULL) {
		fprintf(stderr, "ERROR: can't create window: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	_sdl = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (_sdl == NULL) {
		fprintf(stderr, "ERROR: can't create renderer: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_RendererInfo info;
	SDL_GetRendererInfo(_sdl, &info);
	printf("[SDL] Renderer backend: %s\n", info.name);

	tft.setRender(_sdl);

	// auto start_time = std::chrono::high_resolution_clock::now();
    setup();
    // auto end_time = std::chrono::high_resolution_clock::now();
	// auto time = end_time - start_time;
    // std::cout << time / std::chrono::milliseconds(1) << "ms to run.\n";

	do {
		SDL_Delay(10);
		SDL_PollEvent(&event);
		loop();
	} 
	while (event.type != SDL_QUIT);

	SDL_DestroyRenderer(_sdl);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}