#include <SDL.h>

#include "debug.h"

#define SDL_ERR SDL_GetError()

int main() {
	int ret = -1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ERR("SDL_Init: %s\n", SDL_ERR);
		goto fail;
	}

	SDL_Window *win = SDL_CreateWindow("SDL Tutorial",
	                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					   640, 480,
					   SDL_WINDOW_SHOWN);
	if (win == NULL) {
		ERR("SDL_CreateWindow: %s\n", SDL_ERR);
		goto fail_sdl;
	}

	ret = 0;

	SDL_Surface *scr = SDL_GetWindowSurface(win);
	SDL_FillRect(scr, NULL, SDL_MapRGB(scr->format, 0xff, 0xff, 0xff));
	SDL_UpdateWindowSurface(win);

	SDL_Delay(1000);

	SDL_DestroyWindow(win);

fail_sdl:
	SDL_Quit();
fail:
	return ret;
}
