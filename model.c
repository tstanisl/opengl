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

enum token {
	TOK_EOF,
	TOK_STRING,
	TOK_SLASH,
	TOK_NUMBER,
	TOK_ERROR,
};

#define TOK_SIZE 64

struct lxr {
	FILE *f;
	int line;
	char str[TOK_SIZE];
	float val;
};

static void skip_ws(struct lxr *lxr)
{
	bool comment = false;
	for (;;) {
		int c = fgetc(lxr->f);
		if (comment) {
			if (c == EOF)
				return;
			if (c == '\n') {
				comment = false;
				++lxr->line;
			}
		} else {
			if (c == '#') {
				comment = true;
			} else if (c == '\n') {
				++lxr->line;
			} else if (c != '\\' && !isspace(c)) {
				ungetc(c, lxr->f);
				return;
			}
		}
	}
}

static enum token lxr_get_next(struct lxr *lxr)
{
	skip_ws(lxr);
	int c = fgetc(lxr->f);
	if (c == '/')
		return TOK_SLASH;
	if (c == EOF)
		return TOK_EOF;
	if (c == '.' || c == '+' || isdigit(c)) {
		ungetc(c, lxr->f);
		int ret = fscanf(lxr->f, "%f", &lxr->val);
		if (ret != 1) {
			strcat(lxr->str, "invalid number");
			return TOK_ERROR;
		}
		return TOK_NUMBER;
	}
	if (isgraph(c)) {
		ungetc(c, lxr->f);
		fscanf(lxr->f, "%63s", lxr->str);
		return TOK_STRING;
	}
	sprintf(lxr->str, "unexpected '%c'", c);
	return TOK_ERROR;
}

struct model *model_load(char *path)
{
	FILE *f = fopen(path, "r");
	if (ERR_ON(!f, "fopen(\"%s\"): %s\n", path, ERRSTR))
		return NULL;

	struct lxr lxr = { .line = 1, .f = f };
	for (;;) {
		enum token t = lxr_get_next(&lxr);
		if (t == TOK_EOF) {
			break;
		} else if (t == TOK_ERROR) {
			ERR("%s(%d): %s\n", path, lxr.line, lxr.str);
			goto fail_f;
		} else if (t == TOK_SLASH) {
			printf("%3d: slash\n", lxr.line);
		} else if (t == TOK_NUMBER) {
			printf("%3d: number %g\n", lxr.line, lxr.val);
		} else {
			printf("%3d: string %s\n", lxr.line, lxr.str);
		}
	}
	
	return NULL;
fail_f:
	fclose(f);
	return NULL;
}

