#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

#define ERRSTR strerror(errno)
#define ERR_ON(cond, ...) \
	__err(cond, __FILE__, __LINE__, __VA_ARGS__)
#define ERR(...) ERR_ON(1, __VA_ARGS__)
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

int shader_create(char *path, GLenum type)
{
	int fd = open(path, O_RDONLY);
	if (ERR_ON(fd < 0, "failed to open '%s': %s\n", path, ERRSTR))
		return -1;

	struct stat stat;
	int ret = fstat(fd, &stat);
	if (ERR_ON(ret < 0, "fstat() failed: %s\n", ERRSTR))
		goto fail_fd;

	int size = stat.st_size;

	char *buf = malloc(size + 1);
	if (ERR_ON(!buf, "malloc(%u) failed\n", size))
		goto fail_fd;

	ret = read(fd, buf, size);
	if (ERR_ON(ret < 0, "read() failed\n"))
		goto fail_buf;
	size = ret;

	GLuint id = glCreateShader(type);
	if (ERR_ON(!id, "glCreateShader() failed\n"))
		goto fail_buf;

	glShaderSource(id, 1, (const GLchar **)&buf, NULL);
	glCompileShader(id);

	/* neither fd nor buf is needed any more */
	free(buf);
	close(fd);

	glGetShaderiv(id, GL_COMPILE_STATUS, &ret);
	if (ret == GL_FALSE) {
		static char buf[256];
		glGetShaderInfoLog(id, 256, NULL, buf);
		ERR("failed to compile shader '%s':\n\t%s\n", path, buf);
		glDeleteShader(id);
		return -1;
	}

	return id;

fail_buf:
	free(buf);
fail_fd:
	close(fd);
	return -1;
} 

int program_create(GLuint vertId, GLuint fragId)
{
	GLuint id = glCreateProgram();
	if (ERR_ON(!id, "glCreateProgram() failed\n"))
		return -1;
	glAttachShader(id, vertId);
	glAttachShader(id, fragId);
	glLinkProgram(id);

	int ret;
	glGetProgramiv(id, GL_LINK_STATUS, &ret);
	if (ret == GL_FALSE) {
		static char buf[256];
		glGetProgramInfoLog(id, 256, NULL, buf);
		ERR("failed to link program:\n\t%s\n", buf);
		glDeleteProgram(id);
		return -1;
	}
	return id;
}

int program_create_by_path(char *vert_path, char *frag_path)
{
	int vertId = shader_create(vert_path, GL_VERTEX_SHADER);
	if (ERR_ON(vertId < 0, "shader_create('%s') failed\n", vert_path)) 
		return -1;
	int fragId = shader_create(frag_path, GL_FRAGMENT_SHADER);
	if (ERR_ON(fragId < 0, "shader_create('%s') failed\n", frag_path)) 
		goto fail_vert;
	int progId = program_create(vertId, fragId);
	if (ERR_ON(progId < 0, "program_create() failed\n"))
		goto fail_frag;
	return progId;
fail_frag:
	glDeleteShader(fragId);
fail_vert:
	glDeleteShader(vertId);
	return -1;
}

void loop(struct context *ctx)
{
	int progId = program_create_by_path("simple.vert", "simple.frag");
	if (ERR_ON(progId < 0, "program_create_by_path() failed\n"))
		return;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(progId);

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
