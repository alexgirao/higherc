#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"

void hcns(_p_fatal)(char *file, int line, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	hcns(_p_alloc_fatal_error_happend)();

	fprintf(stderr, "fatal-error: %s: %i: ", file, (int)line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	exit(1);
}
