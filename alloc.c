#include <stdlib.h>
#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"

void *hcns(alloc)(int n)
{
	return malloc(n);
}

void *hcns(alloc_z)(int n)
{
	return calloc(1, n);
}

void hcns(alloc_free)(void *x)
{
	free(x);
}

void *hcns(alloc_re)(void *x, int m, int n)
{
	void *y = hcns(alloc)(n);
	if (!y)	return NULL;
	hcns(bcopyl)(y, m, x);
	hcns(alloc_free)(x);
	return y;
}
