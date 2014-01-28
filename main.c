#include <SDL.h>
#include <stdio.h>
#include <stdarg.h>

int __err(int cond, char *file, int line, char *fmt, ...)
{
	if (!cond)
		return 0;

	va_list va;
	va_start(va, fmt);

	fprintf(stderr, "Error: %s(%d):", file, line);
	vfprintf(stderr, fmt, va);

	va_end(va);

	return cond;
}

#define ERR_ON(cond, ...) \
	__err(cond, __FILE__, __LINE__, __VA_ARGS__)
#define SDLERR_ON(cond) \
	ERR_ON(cond, "%s\n", SDL_GetError())

struct context {
	SDL_Window *win;
};

int win_create(struct context *ctx)
{
	int ret = SDL_Init(SDL_INIT_EVERYTHING);
	if (SDLERR_ON(ret < 0))
		return -1;

	ctx->win = SDL_CreateWindow("OpenGL learning", 0, 0, 640, 480, SDL_WINDOW_SHOWN);
	if (SDLERR_ON(!ctx->win))
		goto fail_sdl;

	return 0;

fail_sdl:
	SDL_Quit();

	return -1;
}

void win_destroy(struct context *ctx)
{
	SDL_DestroyWindow(ctx->win);
	SDL_Quit();
}

int main()
{
	struct context ctx;

	memset(&ctx, 0, sizeof(ctx));

	int ret = win_create(&ctx);
	if (ERR_ON(ret, "win_create failed\n"))
		return EXIT_FAILURE;

	SDL_Delay(2000);

	win_destroy(&ctx);

	return 0;
}

