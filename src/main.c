#include <SDL.h>
#include <GL/gl.h>

#include "debug.h"

#define SDL_ERR SDL_GetError()

struct context {
	SDL_Window *win;
	SDL_GLContext gl_ctx;
};

struct context *context_create(int width, int height) {
	struct context *ctx = malloc(sizeof *ctx);
	if (ERR_ON(ctx == NULL, "malloc failed\n"))
		goto fail;

	ctx->win = SDL_CreateWindow("SDL Tutorial",
				    SDL_WINDOWPOS_UNDEFINED,
				    SDL_WINDOWPOS_UNDEFINED,
				    width, height,
				    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (ERR_ON(ctx->win == NULL, "SDL_CreateWindow: %s\n", SDL_ERR))
		goto fail_ctx;

	ctx->gl_ctx = SDL_GL_CreateContext(ctx->win);
	if (ERR_ON(ctx->gl_ctx == NULL,  "SDL_GL_CreateContext: %s\n", SDL_ERR))
		goto fail_win;

	return ctx;
fail_win:
	SDL_DestroyWindow(ctx->win);
fail_ctx:
	free(ctx);
fail:
	return NULL;
}

void context_destroy(struct context *ctx) {
	SDL_GL_DeleteContext(ctx->gl_ctx);
	SDL_DestroyWindow(ctx->win);
	free(ctx);
}

int context_resize(struct context *ctx) {
	int width, height;
	SDL_GetWindowSize(ctx->win, &width, &height);
	glViewport(0, 0, width, height);
	return 0;
}

void draw(void) {
	puts("draw");
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void loop(struct context *ctx) {
	int done = 0;
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
				done = 1;
		}
		draw();
		SDL_GL_SwapWindow(ctx->win);
	}
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		ERR("SDL_Init: %s\n", SDL_ERR);
		return -1;
	}

	struct context *ctx = context_create(640, 480);
	if (ERR_ON(!ctx, "context_create() failed\n"))
		return -1;
	loop(ctx);

	context_destroy(ctx);
	SDL_Quit();
	return 0;
}
