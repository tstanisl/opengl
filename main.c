#include <SDL.h>
#include <stdio.h>

int main()
{
	int ret;

	ret = SDL_Init(SDL_INIT_EVERYTHING);
	if (ret < 0) {
		fprintf(stderr, "Error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Window *win = SDL_CreateWindow("OpenGL learning", 0, 0, 640, 480, SDL_WINDOW_SHOWN);
	if (!win) {
		fprintf(stderr, "Error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_Delay(2000);

	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}

