#include <stdlib.h>
#include <stdio.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"

#define HC_CONF_ALLOC_OFFENSIVE_MODE

static int _nallocs = 0;

static void _alloc_check(void)
{
	if (_nallocs) {
		/* unbalanced allocations, memory is leaking
		 */
		fprintf(stderr, "Oh, you're screwed It's over. You're flushed: %i.\n", _nallocs);
		fflush(stderr);
	}
}

static int _initialized = 0;
static inline void _initialize(void)
{
	if (_initialized == 0) {
		_initialized = 1;
		atexit(_alloc_check);
	}
}

#ifdef HC_CONF_ALLOC_OFFENSIVE_MODE

void *hcns(alloc)(int n)
{
	int *r;
	_initialize();
	r = malloc(sizeof(int) + n);
	*r = sizeof(int) + n;
	_nallocs++;
	return r + 1;
}

void *hcns(alloc_z)(int n)
{
	int *r;
	_initialize();
	r = calloc(1, sizeof(int) + n);
	*r = sizeof(int) + n;
	_nallocs++;
	return r + 1;
}

void hcns(alloc_free)(void *x)
{
	int *r = x;
	r--;
	/* printf("dirtying %i bytes at %p\n", *r, r); */
	hcns(bset)(r, *r, 0x7f); /* ``offensive programming'' */
	free(r);
	_nallocs--;
}

#else

void *hcns(alloc)(int n)
{
	void *r;
	_initialize();
	r = malloc(n);
	_nallocs++;
	return r;
}

void *hcns(alloc_z)(int n)
{
	void *r;
	_initialize();
	r = calloc(1, n);
	_nallocs++;
	return r;
}

void hcns(alloc_free)(void *x)
{
	free(x);
	_nallocs--;
}

#endif

void *hcns(alloc_re)(void *x, int m, int n)
{
	void *y = hcns(alloc)(n);
	if (!y)	return NULL;
	hcns(bcopyl)(y, m, x);
	hcns(alloc_free)(x);
	return y;
}
