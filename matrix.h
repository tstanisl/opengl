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

static inline void mat4_rotate_z(mat4 M, float rad)
{
	float x, y;
	float c = cosf(rad);
	float s = sinf(rad);
	x = M[0][0]; y = M[1][0];
	M[0][0] = c * x - s * y;
	M[1][0] = s * x + c * y;
	x = M[0][1]; y = M[1][1];
	M[0][1] = c * x - s * y;
	M[1][1] = s * x + c * y;
	x = M[0][2]; y = M[1][2];
	M[0][2] = c * x - s * y;
	M[1][2] = s * x + c * y;
}

#if 0
static inline void mat4_rotate_y(mat4 M, float rad)
{
	float x, y, c, s;
	sincosf(rad, &s, &c);
	x = M[0][0]; y = M[1][0];
	M[0][0] = c * x - s * y;
	M[1][0] = s * x + c * y;
	x = M[0][1]; y = M[1][1];
	M[0][1] = c * x - s * y;
	M[1][1] = s * x + c * y;
	x = M[0][2]; y = M[1][2];
	M[0][2] = c * x - s * y;
	M[1][2] = s * x + c * y;
}
#endif

#endif /* MATRIX_H */
