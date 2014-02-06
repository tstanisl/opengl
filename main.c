#define GL_GLEXT_PROTOTYPES

#include "matrix.h"
#include "debug.h"
#include "model.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

	ret = SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	if (SDLERR_ON(ret < 0))
		goto fail_sdl;

	ctx->win = SDL_CreateWindow("OpenGL learning", 0, 0, 1280, 1024,
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
	buf[size] = 0;

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

static inline void mat4_dump(mat4 M)
{
	puts("-----");
	for (int i = 0; i < 4; ++i)
		printf(" %f %f %f %f\n", M[i][0], M[i][1], M[i][2], M[i][3]);
}

struct camera {
	float x, y, z;
	float theta, azimuth;
};

static int process_event(struct camera *c)
{
	const float speed = 0.1;
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_WINDOWEVENT) {
			if (ev.window.event == SDL_WINDOWEVENT_CLOSE)
				return 0;
		} else if (ev.type == SDL_KEYDOWN) {
			SDL_Keycode k = ev.key.keysym.sym;
			if (k == SDLK_q || k == SDLK_ESCAPE)
				return 0;
			if (k == SDLK_w) {
				c->x += speed * sinf(c->theta);
				c->z -= speed * cosf(c->theta);
			}
			if (k == SDLK_s) {
				c->x -= speed * sinf(c->theta);
				c->z += speed * cosf(c->theta);

			}
			if (k == SDLK_d) {
				c->x += speed * cosf(c->theta);
				c->z += speed * sinf(c->theta);
			}
			if (k == SDLK_a) {
				c->x -= speed * cosf(c->theta);
				c->z -= speed * sinf(c->theta);
			}
		} else if (ev.type == SDL_MOUSEMOTION) {
			SDL_Window* win = SDL_GetWindowFromID(ev.motion.windowID);
			int width, height;
			SDL_GetWindowSize(win, &width, &height);
			c->theta = (((float)ev.motion.x / width) - 0.5) * 4 * M_PI;
			c->azimuth = (((float)ev.motion.y / height) - 0.5) * M_PI;
		}
	}
	return 1;
}

void loop(struct context *ctx)
{
	char *path = "models/suzanne.obj";
	//char *path = "models/chaise.obj";
	struct model *m = model_load(path);
	if (ERR_ON(!m, "model_load(\"%s\") failed\n", path))
		return;

#if 0
	printf("m->element = %d\n", m->n_element);
	for (int i = 0; i < m->n_element; i += 3)
		printf("%d %d %d\n", m->element[i], m->element[i + 1], m->element[i + 2]);
#endif

	int progId = program_create_by_path("simple.vert", "simple.frag");
	if (ERR_ON(progId < 0, "program_create_by_path() failed\n"))
		return;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(progId);

	GLint mvpId = glGetUniformLocation(progId, "MVP");
	if (ERR_ON(mvpId < 0, "failed to bind uniform MVP\n"))
		return;

	GLint mId = glGetUniformLocation(progId, "M");
	if (ERR_ON(mvpId < 0, "failed to bind uniform M\n"))
		return;

	GLuint vb;
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m->vertex[0]) * m->n_vertex, m->vertex,
		GL_STATIC_DRAW);

	int aId;

	aId = glGetAttribLocation(progId, "vpos");
	if (ERR_ON(aId < 0, "pos is not a valid attribute\n"))
		return;
	glEnableVertexAttribArray(aId);
	glVertexAttribPointer(aId, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
		(void*)offsetof(struct vertex, position));
	//glDisableVertexAttribArray(aId);

	aId = glGetAttribLocation(progId, "vnorm");
	if (ERR_ON(aId < 0, "normal is not a valid attribute\n"))
		return;
	glEnableVertexAttribArray(aId);
	glVertexAttribPointer(aId, 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex),
		(void*)offsetof(struct vertex, normal));
	//glDisableVertexAttribArray(aId);

	GLuint eb;
	glGenBuffers(1, &eb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->n_element * sizeof(m->element[0]),
		m->element, GL_STATIC_DRAW);

	mat4 P;
	mat4_perspective(P, 1, 30.0, M_PI / 4, 640.f / 480.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float angle = 0.0f;
	struct camera cam = { .z = 6.5 };
	mat4 VP, MVP;

	while (process_event(&cam)) {
		mat4_identity(VP);
		mat4_translate(VP, -cam.x, -cam.y, -cam.z);
		mat4_rotate_y(VP, cam.theta);
		mat4_rotate_x(VP, cam.azimuth);
		mat4_mul(VP, P);

		mat4_identity(MVP);
		mat4_rotate_z(MVP, angle);
		glUniformMatrix4fv(mId, 1, GL_TRUE, (void*)MVP);
		mat4_mul(MVP, VP);
		glUniformMatrix4fv(mvpId, 1, GL_TRUE, (void*)MVP);

		// drawing
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eb);
		glDrawElements(GL_TRIANGLES, m->n_element, GL_UNSIGNED_INT, (void*)0);

		SDL_GL_SwapWindow(ctx->win);
		angle += 0.01f;
		SDL_Delay(20);
	}

	glDisableVertexAttribArray(0);
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

