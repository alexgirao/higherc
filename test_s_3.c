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

int main(int argc, char **argv)
{
	HC_DEF_S(s);

	/* copyf
	 */

	hcns(s_copyf)(s, "[%i]", 123);
	puts_s(s);

	hcns(s_copyf)(s, "[%f]", 2.718281828459045);
	puts_s(s);

	hcns(s_copyf)(s, "[%s]", "hello world of possibilities!");
	puts_s(s);

	hcns(s_copyf)(s, "[%i][%f][%s]", 123, 2.718281828459045, "hello world of possibilities!");
	puts_s(s);

	/* catf
	 */

	s->len = 0; /* truncate */

	hcns(s_catf)(s, "[%i]", 123);
	puts_s(s);

	hcns(s_catf)(s, "[%f]", 2.718281828459045);
	puts_s(s);

	hcns(s_catf)(s, "[%s]", "hello world of possibilities!");
	puts_s(s);

	hcns(s_catf)(s, "[%i][%f][%s]", 123, 2.718281828459045, "hello world of possibilities!");
	puts_s(s);

	/*
	 */

	hcns(s_free)(s);
	return 0;
}
