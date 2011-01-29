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

#include "higherc/higherc.h"
#include "higherc/byte.h"

int hcns(bchrl)(const void *s, int n, int c)
{
	char ch = c;
	const char *t = s;
	const char *u = t + n;

	for (;;) {
		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;
	}
	return n;
}

int hcns(bchrr)(const void *s, int n, int c)
{
	char ch = c;
	const char *t = (char*)s + n;
	for (;;) {
		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;

		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;

		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;

		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;
	}
	return n;
}

void hcns(bcopyl)(void *to, int n, const void *from)
{
	char *t = to;
	const char *f = from;
	for (;;) {
		if (!n)
			return;
		*t++ = *f++;
		--n;

		if (!n)
			return;
		*t++ = *f++;
		--n;

		if (!n)
			return;
		*t++ = *f++;
		--n;

		if (!n)
			return;
		*t++ = *f++;
		--n;
	}
}

void hcns(bcopyr)(void *to, int n, const void *from)
{
	char *t = (char*)to + n;
	const char *f = (char*)from + n;
	for (;;) {
		if (!n)
			return;
		*--t = *--f;
		--n;

		if (!n)
			return;
		*--t = *--f;
		--n;

		if (!n)
			return;
		*--t = *--f;
		--n;

		if (!n)
			return;
		*--t = *--f;
		--n;
	}
}

int hcns(bdiff)(const void *s, int n, const void *t)
{
	const char *x=s;
	const char *y=t;
	for (;;) {
		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;

		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;

		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;

		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;
	}
	return ((int)(unsigned char)*x)
	    - ((int)(unsigned char)*y);
}

void hcns(bzero)(void *s, int n)
{
	char *x=s;
	for (;;) {
		if (!n)
			break;
		*x++ = 0;
		--n;

		if (!n)
			break;
		*x++ = 0;
		--n;

		if (!n)
			break;
		*x++ = 0;
		--n;

		if (!n)
			break;
		*x++ = 0;
		--n;
	}
}

void hcns(brev)(void *x, int n)
{
	char *b = x;
	char *e = b + n - 1;
	while (b < e) {
		char t = *b;
		*b++ = *e;
		*e-- = t;
	}
}

void hcns(bset)(void *x, int n, char c)
{
	char *b = x;
	while (n--) {
		*b++ = c;
	}
}
