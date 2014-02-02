#ifndef DEBUG_H
#define DEBUG_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static inline int __err(int cond, char *file, int line, char *fmt, ...)
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

#endif /* DEBUG_H */
