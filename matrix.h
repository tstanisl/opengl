#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include <string.h>

typedef float mat4[4][4];

static inline void mat4_identity(mat4 M)
{
	memset(M, 0, sizeof(mat4));
	M[0][0] = M[1][1] = M[2][2] = M[3][3] = 1.0;
}

static inline void mat4_scale(mat4 M, float sx, float sy, float sz)
{
	M[0][0] *= sx; M[0][1] *= sx; M[0][2] *= sx;
	M[1][0] *= sy; M[1][1] *= sy; M[1][2] *= sy;
	M[2][0] *= sz; M[2][1] *= sz; M[2][2] *= sz;
}

static inline void mat4_translate(mat4 M, float tx, float ty, float tz)
{
	M[0][0] += M[3][0] * tx;
	M[0][1] += M[3][1] * tx;
	M[0][2] += M[3][2] * tx;
	M[0][3] += M[3][3] * tx;
	M[1][0] += M[3][0] * ty; 
	M[1][1] += M[3][1] * ty;
	M[1][2] += M[3][2] * ty;
	M[1][3] += M[3][3] * ty;
	M[2][0] += M[3][0] * tz;
	M[2][1] += M[3][1] * tz;
	M[2][2] += M[3][2] * tz;
	M[2][3] += M[3][3] * tz;
}

#define __MAT4_ROTATE(__axis,__x,__y) \
static inline void mat4_rotate_ ## __axis(mat4 M, float rad) \
{ \
	float x, y; \
	float c = cosf(rad); \
	float s = sinf(rad); \
	x = M[__x][0]; y = M[__y][0]; \
	M[__x][0] = c * x - s * y; \
	M[__y][0] = s * x + c * y; \
	x = M[__x][1]; y = M[__y][1]; \
	M[__x][1] = c * x - s * y; \
	M[__y][1] = s * x + c * y; \
	x = M[__x][2]; y = M[__y][2]; \
	M[__x][2] = c * x - s * y; \
	M[__y][2] = s * x + c * y; \
}

__MAT4_ROTATE(x, 1, 2)
__MAT4_ROTATE(y, 2, 0)
__MAT4_ROTATE(z, 0, 1)

#endif /* MATRIX_H */
