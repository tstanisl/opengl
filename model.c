#include "model.h"
#include "debug.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define VMAX 1024
#define HSIZE (1 << 10)

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

static struct ivertex ivertex[VMAX];
static int n_ivertex;
static struct ivertex *ivhash[HSIZE];

static unsigned ivertex_hash(struct ivertex *iv)
{
	return iv->normal + iv->texture * 1759 + iv->position * 43517;
}

static struct ivertex *ivertex_find(struct ivertex *ref)
{
	unsigned idx = ivertex_hash(ref) & (HSIZE - 1);
	struct ivertex *iv;
	for (iv = ivhash[idx]; iv; iv = iv->next) {
		if (iv->position != ref->position)
			continue;
		if (iv->texture != ref->texture)
			continue;
		if (iv->normal != ref->normal)
			continue;
		return iv;
	}
	if (ERR_ON(n_ivertex >= VMAX, "too many ivertetices\n"))
		return NULL;
	iv = &ivertex[n_ivertex++];
	*iv = *ref;
	iv->next = ivhash[idx];
	ivhash[idx] = iv;
	return iv;
}

enum token {
	TOK_EOF,
	TOK_EOL,
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

static int skip_ws(struct lxr *lxr)
{
	enum {
		ST_NONE,
		ST_COMMENT,
		ST_BSLASH,
	} st = ST_NONE;
	for (;;) {
		int c = fgetc(lxr->f);
		if (c == EOF)
			return 0;
		if (st == ST_NONE) {
			if (c == '#') {
				st = ST_COMMENT;
			} else if (c == '\\') {
				st = ST_BSLASH;
			} else if (c != ' ' && c != '\t') {
				ungetc(c, lxr->f);
				return 0;
			}
		} else if (st == ST_COMMENT) {
			if (c == '\n') {
				st = ST_NONE;
				++lxr->line;
			}
		} else if (st == ST_BSLASH) {
			if (c == '\n') {
				st = ST_NONE;
				++lxr->line;
			} else if (!isspace(c)) {
				strcat(lxr->str, "non-space after \\");
				return -1;
			}
		}
	}
}

static enum token lxr_get_next(struct lxr *lxr)
{
	int ret = skip_ws(lxr);
	if (ret)
		return TOK_ERROR;
	int c = fgetc(lxr->f);
	if (c == '\n') {
		++lxr->line;
		return TOK_EOL;
	}
	if (c == '/')
		return TOK_SLASH;
	if (c == EOF)
		return TOK_EOF;
	if (c == '.' || c == '-' || c == '+' || isdigit(c)) {
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
#if 1
	enum token t = lxr->next;
	if (t == TOK_ERROR) {
		printf("%3d: error: %s\n", lxr->line, lxr->str);
	} else if (t == TOK_EOF) {
		printf("%3d: eof\n", lxr->line);
	} else if (t == TOK_EOL) {
		printf("%3d: end-of-line\n", lxr->line);
	} else if (t == TOK_SLASH) {
		printf("%3d: slash\n", lxr->line);
	} else if (t == TOK_NUMBER) {
		printf("%3d: number %g\n", lxr->line, lxr->val);
	} else {
		printf("%3d: string %s\n", lxr->line, lxr->str);
	}
#endif
}

static bool lxr_eol(struct lxr *lxr)
{
	return lxr->next == TOK_EOL || lxr->next == TOK_EOF;
}

static int model_process_v(struct lxr *lxr)
{
	printf("%s\n", __func__);
	lxr_consume(lxr);
	if (ERR_ON(n_position >= VMAX, "too many vertices\n"))
		return -1;
	for (int i = 0; i < 3; ++i) {
		if (lxr->next != TOK_NUMBER) {
			ERR("(%d) after v number expected at position %d\n",
				lxr->line, i + 1);
			return -1;
		}
		position[n_position][i] = lxr->val;
		lxr_consume(lxr);
	}
	/* skip fourth coordinate */
	if (lxr->next == TOK_NUMBER)
		lxr_consume(lxr);
	if (ERR_ON(!lxr_eol(lxr), "(%d) v must end with newline\n", lxr->line))
		return -1;
	lxr_consume(lxr);
	++n_position;
	return 0;
}

static int model_process_vt(struct lxr *lxr)
{
	printf("%s\n", __func__);
	lxr_consume(lxr);
	if (ERR_ON(n_texture >= VMAX, "too many texture points\n"))
		return -1;
	if (lxr->next != TOK_NUMBER) {
		ERR("(%d) vt: coordinate expected\n", lxr->line);
		return -1;
	}
	texture[n_texture][0] = lxr->val;
	lxr_consume(lxr);
	/* optional second coordinate */
	if (lxr->next == TOK_NUMBER) {
		texture[n_texture][1] = lxr->val;
		lxr_consume(lxr);
	} else {
		texture[n_texture][1] = 0.0f;
	}
	/* skip third coordinate */
	if (lxr->next == TOK_NUMBER)
		lxr_consume(lxr);
	if (ERR_ON(!lxr_eol(lxr), "(%d) vt must end with newline\n", lxr->line))
		return -1;
	lxr_consume(lxr);
	++n_texture;
	return 0;
}

static int model_process_vn(struct lxr *lxr)
{
	printf("%s\n", __func__);
	lxr_consume(lxr);
	if (ERR_ON(n_normal >= VMAX, "too many normals\n"))
		return -1;
	for (int i = 0; i < 3; ++i) {
		if (lxr->next != TOK_NUMBER) {
			ERR("(%d) after v number expected at normal %d\n",
				lxr->line, i + 1);
			return -1;
		}
		normal[n_normal][i] = lxr->val;
		lxr_consume(lxr);
	}
	if (ERR_ON(!lxr_eol(lxr), "(%d) vn must end with newline\n", lxr->line))
		return -1;
	lxr_consume(lxr);
	++n_normal;
	return 0;
}

static int model_process_unknown(struct lxr *lxr)
{
	printf("(%d): %s(%s)\n", lxr->line, __func__, lxr->str);
	lxr_consume(lxr);
	while (!lxr_eol(lxr))
		lxr_consume(lxr);
	lxr_consume(lxr);
	return 0;
}

static int model_process_ivertex(struct lxr *lxr, struct ivertex *iv)
{
	memset(iv, 0, sizeof(*iv));
	/* TODO: add checking if integer is used */
	if (lxr->next != TOK_NUMBER)
		return -1;
	iv->position = (int)lxr->val;
	if (iv->position < 0)
		iv->position = n_position - iv->position + 1;
	if (ERR_ON(iv->position <= 0, "(%d): invalid position\n", lxr->line))
		return -1;
	if (lxr->next != TOK_SLASH)
		return 0;
	lxr_consume(lxr);
	if (lxr->next == TOK_NUMBER) {
		iv->texture = lxr->val;
		if (iv->texture < 0)
			iv->texture = n_texture - iv->texture + 1;
		if (iv->texture < 0) {
			ERR("(%d): invalid texture\n", lxr->line);
			return -1;
		}
		lxr_consume(lxr);
	}
	if (lxr->next != TOK_SLASH)
		return 0;
	if (lxr->next != TOK_NUMBER) {
		ERR("(%d): invalid vertex format\n", lxr->line);
		return -1;
	}
	iv->normal = lxr->val;
	if (iv->normal < 0)
		iv->normal = n_normal - iv->normal + 1;
	if (iv->normal < 0) {
		ERR("(%d): invalid normal\n", lxr->line);
		return -1;
	}
	lxr_consume(lxr);
	return 0;
}

static bool ivertex_ok(struct ivertex *iv, struct ivertex *ref)
{
	if (iv->position && !ref->position)
		return false;
	if (!iv->position && ref->position)
		return false;
	if (iv->texture && !ref->texture)
		return false;
	if (!iv->texture && ref->texture)
		return false;
	if (iv->normal && !ref->normal)
		return false;
	if (!iv->normal && ref->normal)
		return false;
	return true;
}

static int model_process_face(struct lxr *lxr)
{
	printf("%s\n", __func__);
	lxr_consume(lxr);
	int count;
	struct ivertex *ref = NULL;
	for (count = 0; !lxr_eol(lxr); ++count) {
		struct ivertex iv;
		int ret = model_process_ivertex(lxr, &iv);
		if (ret) {
			ERR("(%d): f: at ivertex %d\n", lxr->line, count + 1);
			return -1;
		}
		if (ref && !ivertex_ok(&iv, ref)) {
			ERR("(%d): f: inconsistent ivertex format\n", lxr->line);
			return -1;
		}
		struct ivertex *ptr;
		ptr = ivertex_find(&iv);
		if (ERR_ON(!ptr, "ivertex_find failed\n"))
			return -1;
		ref = ptr;
		/* TODO: add vertex to elements */
	}
	/* process new-line */
	lxr_consume(lxr);
	return 0;
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
		if (ERR_ON(t != TOK_STRING, "(%d) syntax error\n", lxr.line))
			return -1;
		if (strcmp(lxr.str, "v") == 0)
			ret = model_process_v(&lxr);
		else if (strcmp(lxr.str, "vt") == 0)
			ret = model_process_vt(&lxr);
		else if (strcmp(lxr.str, "vn") == 0)
			ret = model_process_vn(&lxr);
		else if (strcmp(lxr.str, "f") == 0)
			ret = model_process_face(&lxr);
		else
			ret = model_process_unknown(&lxr);
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

