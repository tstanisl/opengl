#include "model.h"
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

struct model *model_load(char *path)
{
}

