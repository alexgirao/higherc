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
#include "higherc/str.h"

int schrl(const char *s, int c)
{
	char ch;
	const char *t;

	ch = c;
	t = s;
	for (;;) {
		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

	}
	return t - s;
}

int schrr(const char *s, int c)
{
	char ch;
	const char *t;
	const char *u;

	ch = c;
	t = s;
	u = 0;
	for (;;) {
		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;
	}
	if (u)
		return u - s;
	return t - s;
}

int scopy(char *s, const char *t)
{
	char *tmp = s;
	for (;;) {
		if (!(*s = *t))
			break;
		++s;
		++t;

		if (!(*s = *t))
			break;
		++s;
		++t;

		if (!(*s = *t))
			break;
		++s;
		++t;
		
		if (!(*s = *t))
			break;
		++s;
		++t;
	}
	return s - tmp;
}

int sdiff(const char *s, const char *t)
{
	char x;

	for (;;) {
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;
	}

	return ((int)(unsigned char)x)
	    - ((int)(unsigned char)*t);
}

int sdiffn(const char *s, const char *t, int len)
{
	char x;

	for (;;) {
		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;
	}

	return ((int)(unsigned char)x)
	    - ((int)(unsigned char)*t);
}

int slen(const char *s)
{
	const char *t;

	t = s;
	for (;;) {
		if (!*t)
			return t - s;
		++t;

		if (!*t)
			return t - s;
		++t;

		if (!*t)
			return t - s;
		++t;

		if (!*t)
			return t - s;
		++t;
	}
}

bool sstart(const char *s, const char *t)
{
	char x;

	for (;;) {
		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;

		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;

		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;

		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;
	}
}
