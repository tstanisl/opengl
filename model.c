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
	enum token next;
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

void lxr_consume(struct lxr *lxr)
{
	lxr->next = lxr_get_next(lxr);
}

static int model_process_obj(FILE *f)
{
	struct lxr lxr = { .line = 1, .f = f };
	lxr_consume(&lxr);
	int ret = 0;
	while (ret == 0) {
		enum token t = lxr.next;
		if (t == TOK_EOF)
			return 0;
#if 0
		if (ERR_ON(t != TOK_STRING, "syntax error at line %d\n", lxr.line))
			return -1;
		if (strcmp(lxr.str, "v") == 0)
			ret = model_process_v(&lxr);
		else if (strcmp(lxr.str, "vt") == 0)
			ret = model_process_vt(&lxr);
		else if (strcmp(lxr.str, "vn") == 0)
			ret = model_process_vn(&lxr);
		else if (strcmp(lxr.str, "f") == 0)
			ret = model_process_face(&lxr);
#endif
#if 1
		if (t == TOK_ERROR) {
			ERR("%3d: error: %s\n", lxr.line, lxr.str);
			return -1;
		} else if (t == TOK_SLASH) {
			printf("%3d: slash\n", lxr.line);
		} else if (t == TOK_NUMBER) {
			printf("%3d: number %g\n", lxr.line, lxr.val);
		} else {
			printf("%3d: string %s\n", lxr.line, lxr.str);
		}
		lxr_consume(&lxr);
#endif
	}
	return -1;
}

struct model *model_load(char *path)
{
	FILE *f = fopen(path, "r");
	if (ERR_ON(!f, "fopen(\"%s\"): %s\n", path, ERRSTR))
		return NULL;

	int ret = model_process_obj(f);
	if (ERR_ON(ret, "failed to process '%s'\n", path))
		goto fail_f;

	fclose(f);
	return NULL;
fail_f:
	fclose(f);
	return NULL;
}

