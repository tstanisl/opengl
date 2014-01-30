#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <stdarg.h>

int __err(int cond, char *file, int line, char *fmt, ...)
{
	if (!cond)
		return 0;

	va_list va;
	va_start(va, fmt);

	fprintf(stderr, "Error: %s(%d): ", file, line);
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
	SDL_Renderer *render;
};

int win_create(struct context *ctx)
{
	int ret = SDL_Init(SDL_INIT_EVERYTHING);
	if (SDLERR_ON(ret < 0))
		return -1;

	ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	if (SDLERR_ON(ret < 0))
		goto fail_sdl;

	ret = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	if (SDLERR_ON(ret < 0))
		goto fail_sdl;

	ctx->win = SDL_CreateWindow("OpenGL learning", 0, 0, 640, 480,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (SDLERR_ON(!ctx->win))
		goto fail_sdl;

	ctx->render = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_ACCELERATED);
	if (SDLERR_ON(!ctx->render))
		goto fail_win;

	return 0;

fail_win:
	SDL_DestroyWindow(ctx->win);
fail_sdl:
	SDL_Quit();

	return -1;
}

void win_destroy(struct context *ctx)
{
	SDL_DestroyRenderer(ctx->render);
	SDL_DestroyWindow(ctx->win);
	SDL_Quit();
}

void loop(struct context *ctx)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat vert[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
	};

	GLuint vb;
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0L);

	// drawing
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);

	SDL_GL_SwapWindow(ctx->win);

	SDL_Delay(2000);
}

int main()
{
	struct context ctx;

	memset(&ctx, 0, sizeof(ctx));

	int ret = win_create(&ctx);
	if (ERR_ON(ret, "win_create failed\n"))
		return EXIT_FAILURE;

	loop(&ctx);

	win_destroy(&ctx);

	return 0;
}

