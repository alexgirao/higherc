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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"
#include "higherc/str.h"
#include "higherc/s.h"

static void print_s(char *prefix, HC_ST_S *s, char *suffix)
{
	if (prefix) printf("%s", prefix);
	fwrite(s->s, s->len, 1, stdout);
	if (suffix) printf("%s", suffix);
}

int main(int argc, char **argv)
{
	int i;
	HC_DEF_S(s);
	HC_DEF_S(repr);

	/*
	 */

	for (i=0; i<=100; i++) {
		hcns(s_catc)(s, i);
	}
	hcns(s_repr)(repr, s);

	print_s("[", repr, "]\n");

	/*
	 */

	repr->len = 0;
	hcns(s_reprz)(repr, "\x1\x2\x3");

	print_s("[", repr, "]\n");

	/*
	 */

	hcns(s_free)(repr);
	hcns(s_free)(s);

	return 0;
}
