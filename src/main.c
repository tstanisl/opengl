#include <SDL.h>
#include <GL/gl.h>

#include "debug.h"

#define SDL_ERR SDL_GetError()

SDL_Window* init_win(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ERR("SDL_Init: %s\n", SDL_ERR);
		return NULL;
	}

	SDL_Window *win = SDL_CreateWindow("SDL Tutorial",
	                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
					   640, 480,
					   SDL_WINDOW_SHOWN);
	if (win == NULL) {
		ERR("SDL_CreateWindow: %s\n", SDL_ERR);
		SDL_Quit();
		return NULL;
	}

	return win;
}

void deinit_win(SDL_Window* win) {
	SDL_DestroyWindow(win);
	SDL_Quit();
}

int init_gl(SDL_Window *win) {
	int width, height;
	SDL_GetWindowSize(win, &width, &height);
	glViewport(0, 0, width, height);
	return 0;
}

void draw(void) {
}

void loop(void) {
	int done = 0;
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
				done = 1;
		}
	}
}

int main() {
	SDL_Window* win = init_win();
	if (win == NULL)
		return -1;
	init_gl(win);
	loop();
	deinit_win(win);
	return 0;
}
