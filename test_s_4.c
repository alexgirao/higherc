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

void puts_s(HC_ST_S *s)
{
	assert(s->s);
	fwrite(s->s, s->len, 1, stdout);
	fputs("\n", stdout);
}

void puts_s3(char *prefix, HC_ST_S *s, char *suffix)
{
	assert(s->s);
	fputs(prefix, stdout);
	fwrite(s->s, s->len, 1, stdout);
	fputs(suffix, stdout);
}

int main(int argc, char **argv)
{
	HC_DEF_S(s);

	/* lchr/rchr
	 */

	s_copyz(s, "/alpha");
	assert(s_lchr(s, '/') == 0 && s_rchr(s, '/') == 0);

	s_copyz(s, "alpha/bravo");
	assert(s_lchr(s, '/') == 5 && s_rchr(s, '/') == 5);

	s_copyz(s, "alpha/");
	assert(s_lchr(s, '/') == 5 && s_rchr(s, '/') == 5);

	s_copyz(s, "alpha");
	assert(s_lchr(s, '/') == 5 && s_rchr(s, '/') == 5);

	/* pattern: occurrence/no occurrence
	 */

	s_copyz(s, "alpha/bravo");
	assert(s_lchr(s, '/') < s->len);
	assert(s_lchr(s, '?') == s->len);

	/* pattern: detect and remove a section
	 */

	{
		int i;
		s_copyz(s, "alpha/Attic/bravo");
		if ((i = s_rchr(s, '/')) < s->len && i > 6 && sdiffn(s->s + i - 6, "/Attic/", 7) == 0) {
			printf("%i chars before match\n", i);
			printf("%i chars after match\n", s->len - i - 1);

			s_shiftl2(s, i - 6, 6, 'Z');
			s->len -= 6;

			assert(s_diffz(s, "alpha/bravo") == 0);
		}
	}

	/*
	 */

	puts("ok");

	s_free(s);
	return 0;
}
