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
	HC_DEF_S(s);

	/* shift right
	 */

	s_catz(s, "alphaXYZbravo");
	print_s("[", s, "]\n");

	s_shiftr(s, 5, 8, 1, '0');
	assert(s_diffz(s, "alpha0XYbravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftr(s, 5, 8, 1, '9');
	assert(s_diffz(s, "alpha90Xbravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftr(s, 7, 8, 1, '2');
	assert(s_diffz(s, "alpha902bravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftr(s, 5, 9, 3, '1');
	assert(s_diffz(s, "alpha1119ravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftr(s, 5, -1, 3, '2');
	assert(s_diffz(s, "alpha2221119o") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftr(s, 5, s->len, 3, '3');
	assert(s_diffz(s, "alpha33322211") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftr(s, 5, s->len + 1, 3, '4');
	assert(s_diffz(s, "alpha444333222") == 0 && s->len == 14);
	print_s("[", s, "]\n");

	s_shiftr(s, 5, s->len + 3, 3, '5');
	assert(s_diffz(s, "alpha555444333222") == 0 && s->len == 17);
	print_s("[", s, "]\n");

	s_shiftr(s, 0, 5, 5, '6');
	assert(s_diffz(s, "66666555444333222") == 0 && s->len == 17);
	print_s("[", s, "]\n");

	/* shift left
	 */

	s_copyz(s, "alphaXYZbravo");
	print_s("[", s, "]\n");

	s_shiftl(s, 5, 8, 1, '0');
	assert(s_diffz(s, "alphaYZ0bravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, 5, 8, 1, '1');
	assert(s_diffz(s, "alphaZ01bravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, 5, 8, 1, '2');
	assert(s_diffz(s, "alpha012bravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, 5, s->len, 1, '3');
	assert(s_diffz(s, "alpha12bravo3") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, 5, -1, 1, '4');
	assert(s_diffz(s, "alpha2bravo43") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, 0, s->len, 1, '5');
	assert(s_diffz(s, "lpha2bravo435") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, -1, s->len, 1, '6');
	assert(s_diffz(s, "lpha2bravo436") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, -2, s->len, 2, '7');
	assert(s_diffz(s, "lpha2bravo477") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl(s, 0, s->len, s->len, '5');
	assert(s_diffz(s, "5555555555555") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl2(s, 0, 1, '6');
	assert(s_diffz(s, "5555555555556") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	s_shiftl2(s, 0, 1, '7');
	assert(s_diffz(s, "5555555555567") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	/* shift right
	 */

	s_shiftr2(s, 0, 1, '4');
	assert(s_diffz(s, "45555555555567") == 0 && s->len == 14);
	print_s("[", s, "]\n");

	s_shiftr2(s, 0, 1, '3');
	assert(s_diffz(s, "345555555555567") == 0 && s->len == 15);
	print_s("[", s, "]\n");

	s_shiftr(s, 0, s->len, 1, '2');
	assert(s_diffz(s, "234555555555556") == 0 && s->len == 15);
	print_s("[", s, "]\n");

	s_shiftr2(s, -1, 3, '1');
	assert(s_diffz(s, "234555555555551116") == 0 && s->len == 18);
	print_s("[", s, "]\n");

	s_free(s);

	return 0;
}
