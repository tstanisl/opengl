#include "model.h"
#include "debug.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define VMAX 1024
#define HSIZE (1 << 16)

struct ivertex {
	int position;
	int texture;
	int normal;
	struct ivertex *next;
};

static float position[VMAX][3];
static int n_position;
static float texture[VMAX][2];
static int n_texture;
static float normal[VMAX][3];
static int n_normal;

static struct ivertex *hash[HSIZE];
static struct ivertex ivertex[VMAX];

static int skip_ws(FILE *f, int line)
{
	bool comment = false;
	for (;;) {
		int c = fgetc(f);
		if (comment) {
			if (c == EOF)
				return line;
			if (c == '\n') {
				comment = false;
				++line;
			}
		} else {
			if (c == '#') {
				comment = true;
			} else if (c == '\n') {
				++line;
			} else if (c != '\\' && !isspace(c)) {
				ungetc(c, f);
				return line;
			}
		}
	}
}

enum token {
	TOK_EOF,
	TOK_STRING,
	TOK_SLASH,
	TOK_NUMBER,
	TOK_ERROR,
};

#define TOK_SIZE 64
union payload {
	float val;
	char str[TOK_SIZE];
};

static enum token get_next(FILE *f, union payload *payload, int *line)
{
	*line = skip_ws(f, *line);
	int c = fgetc(f);
	if (c == '/')
		return TOK_SLASH;
	if (c == EOF)
		return TOK_EOF;
	if (c == '.' || c == '+' || isdigit(c)) {
		ungetc(c, f);
		int ret = fscanf(f, "%f", &payload->val);
		if (ret != 1) {
			strcat(payload->str, "invalid number");
			return TOK_ERROR;
		}
		return TOK_NUMBER;
	}
	if (isgraph(c)) {
		ungetc(c, f);
		fscanf(f, "%63s", payload->str);
		return TOK_STRING;
	}
	sprintf(payload->str, "unexpected '%c'", c);
	return TOK_ERROR;
}

struct model *model_load(char *path)
{
	FILE *f = fopen(path, "r");
	if (ERR_ON(!f, "fopen(\"%s\"): %s\n", path, ERRSTR))
		return NULL;

	int line = 1;
	for (;;) {
		union payload p;
		enum token t = get_next(f, &p, &line);
		if (t == TOK_EOF) {
			break;
		} else if (t == TOK_ERROR) {
			ERR("%s(%d): %s\n", path, line, p.str);
			goto fail_f;
		} else if (t == TOK_SLASH) {
			printf("%3d: slash\n", line);
		} else if (t == TOK_NUMBER) {
			printf("%3d: number %g\n", line, p.val);
		} else {
			printf("%3d: string %s\n", line, p.str);
		}
	}
	
	return NULL;
fail_f:
	fclose(f);
	return NULL;
}

