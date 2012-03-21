/*
 * Copyright (C) 2010 Alexandre Girao <alexgirao@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"

#define HC_CONF_ALLOC_OFFENSIVE_MODE

static int _n_alloc = 0;
static int _n_free = 0;
static int _dont_report = 0;

static void _alloc_check(void)
{
	if ((_n_alloc - _n_free) && !_dont_report) {
		/* unbalanced allocations, memory is leaking
		 */
		fprintf(stderr, "Oh, you're screwed It's over. You're flushed: %i unallocated items out of %i allocations.\n", _n_alloc - _n_free, _n_alloc);
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

void *_p_alloc(int n)
{
	int *r;
	_initialize();
	r = malloc(sizeof(int) + n);
	if (r) {
		*r = sizeof(int) + n;
		_n_alloc++;
		return r + 1;
	}
	return NULL;
}

void *_p_alloc_z(int n)
{
	int *r;
	_initialize();
	r = calloc(1, sizeof(int) + n);
	if (r) {
		*r = sizeof(int) + n;
		_n_alloc++;
		return r + 1;
	}
	return NULL;
}

void _p_alloc_free(void *x)
{
	int *r = x;
	r--;
	b_set(r, *r, 0x7f); /* ``offensive programming'' */
	free(r);
	_n_free++;
}

#else

void *_p_alloc(int n)
{
	void *r;
	_initialize();
	r = malloc(n);
	_n_alloc++;
	return r;
}

void *_p_alloc_z(int n)
{
	void *r;
	_initialize();
	r = calloc(1, n);
	_n_alloc++;
	return r;
}

void _p_alloc_free(void *x)
{
	free(x);
	_n_free--;
}

#endif

void _p_alloc_fatal_error_happend(void)
{
	_dont_report = 1;
}

void *alloc_re(void *x, int m, int n)
{
	void *y = _p_alloc(n);
	if (!y)	return NULL;
	b_copyl(y, m, x);
	_p_alloc_free(x);
	return y;
}

int alloc_delta(void)
{
	return _n_alloc - _n_free;
}
