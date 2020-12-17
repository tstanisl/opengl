#include <SDL.h>

#include "debug.h"

int main() {
	SDL_Window *win;
	SDL_Surface *scr;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ERR("SDL_Init: failed: %s\n", SDL_GetError());
		goto fail;
	}

	return 0;

fail:
	return -1;
}
