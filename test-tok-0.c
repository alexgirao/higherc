#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "higherc/higherc.h"
#include "higherc/tok.h"

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define FATAL(...)       fatal(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define FATAL(fmt...)   fatal(__FILE__, __LINE__, fmt)
#endif

static void fatal(char *file, int line, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	fprintf(stderr, "fatal-error: %s: %i: ", file, (int)line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	exit(-1);
}

int main(int argc, char **argv)
{
	
	return 0;
}
