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

	hcns(s_catz)(s, "hello world of possibilities!");
	HC_SAFE_CSTR(s);

	fprintf(stdout, "[%i][%i][%s]\n", s->len, s->a, s->s);

	s->len = 0;  /* truncate string */

	hcns(s_catn)(s, "hi ", 3);
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_copyc)(s, 'f');
	hcns(s_catz)(s, "olk");
	hcns(s_catc)(s, 's');
	hcns(s_catc)(s, '!');
	hcns(s_catc)(s, '\n');
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_upper)(s);
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_lower)(s);
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_copyz)(s, "aa");

	assert(hcns(sdiff)("aa", "aa") == 0);
	assert(hcns(s_diffz)(s, "aa") == 0);
	assert(hcns(s_diffn)(s, "aa", hcns(slen("aa"))) == 0);

	assert(hcns(sdiff)("aa", "aaz") < 0);
	assert(hcns(s_diffz)(s, "aaz") < 0);
	assert(hcns(s_diffn)(s, "aaz", hcns(slen("aaz"))) < 0);

	assert(hcns(sdiff)("aa", "a0z") > 0);
	assert(hcns(s_diffz)(s, "a0z") > 0);
	assert(hcns(s_diffn)(s, "a0z", hcns(slen("a0z"))) > 0);

	assert(hcns(sdiff)("aa", "0") > 0);
	assert(hcns(s_diffz)(s, "0") > 0);
	assert(hcns(s_diffn)(s, "0", hcns(slen("0"))) > 0);

	assert(hcns(sdiff)("aa", "0zz") > 0);
	assert(hcns(s_diffz)(s, "0zz") > 0);
	assert(hcns(s_diffn)(s, "0zz", hcns(slen("0zz"))) > 0);

	assert(hcns(sdiff)("aa", "a") > 0);
	assert(hcns(s_diffz)(s, "a") > 0);
	assert(hcns(s_diffn)(s, "a", hcns(slen("a"))) > 0);

	assert(hcns(sdiff)("aa", "a0") > 0);
	assert(hcns(s_diffz)(s, "a0") > 0);
	assert(hcns(s_diffn)(s, "a0", hcns(slen("a0"))) > 0);

	hcns(s_free)(s);

	/*
	 */

	assert(hcns(s_cat_u4_hex)(s, 0) == 1);
	assert(hcns(s_diffz)(s, "0") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_i4_hex)(s, 0) == 1);
	assert(hcns(s_diffz)(s, "0") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_u4_hex)(s, 0xffffffff) == 8);
	assert(hcns(s_diffz)(s, "ffffffff") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_i4_hex)(s, (hcns(i4))0xffffffff) == 2);
	assert(hcns(s_diffz)(s, "-1") == 0);
	print_s("[", s, "]\n");

	hcns(s_free)(s);

	/*
	 */

	assert(hcns(s_cat_u4_dec)(s, 0) == 1);
	assert(hcns(s_diffz)(s, "0") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_i4_dec)(s, 0) == 1);
	assert(hcns(s_diffz)(s, "0") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_u4_dec)(s, 0xffffffff) == 10);
	assert(hcns(s_diffz)(s, "4294967295") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_i4_dec)(s, (hcns(i4))0xffffffff) == 2);
	assert(hcns(s_diffz)(s, "-1") == 0);
	print_s("[", s, "]\n");

	hcns(s_free)(s);

	/*
	 */

	assert(hcns(s_cat_u4_base36)(s, 0) == 1);
	assert(hcns(s_diffz)(s, "0") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_i4_base36)(s, 0) == 1);
	assert(hcns(s_diffz)(s, "0") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_u4_base36)(s, 0xffffffff) == 7);
	assert(hcns(s_diffz)(s, "1z141z3") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	assert(hcns(s_cat_i4_base36)(s, (hcns(i4))0xffffffff) == 2);
	assert(hcns(s_diffz)(s, "-1") == 0);
	print_s("[", s, "]\n");

	hcns(s_free)(s);

	/*
	 */

	{
		void *out;
		int putlen;
		HC_DEF_S(t);

		hcns(s_copyz)(s, "hello world of possibilities!");
		putlen = hcns(s_putlen)(s);

		HC_ALLOC(out, putlen);

		assert(hcns(s_put)(s, out) == putlen);
		assert(hcns(s_get)(t, out) == putlen);
		assert(hcns(s_diff)(s, t) == 0);

		s->len = 0;
		hcns(s_reprn)(s, out, putlen);
		print_s("[", s, "]\n");

		HC_FREE(out);
		hcns(s_free)(t);
	}

	/*
	 */

	hcns(s_free)(s);

	return 0;
}