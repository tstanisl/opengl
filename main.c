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
	SDL_Quit();
	return 0;
}

