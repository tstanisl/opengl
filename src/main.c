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
					   SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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

SDL_GLContext init_gl(SDL_Window *win) {
	SDL_GLContext ctx = SDL_GL_CreateContext(win);
	if (ctx == NULL) {
		ERR("SDL_GL_CreateContext: %s\n", SDL_ERR);
		return NULL;
	}

	int width, height;
	SDL_GetWindowSize(win, &width, &height);
	glViewport(0, 0, width, height);

	return ctx;
}

void draw(void) {
	puts("draw");
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void loop(SDL_Window *win) {
	int done = 0;
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
				done = 1;
		}
		draw();
		SDL_GL_SwapWindow(win);
	}
}

int main() {
	SDL_Window* win = init_win();
	if (win == NULL)
		return -1;
	init_gl(win);
	loop(win);
	deinit_win(win);
	return 0;
}
