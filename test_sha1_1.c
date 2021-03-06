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
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"
#include "higherc/sha1.h"
#include "higherc/s.h"

int main()
{
	HC_DEF_S(msg);
	HC_DEF_S(s);

	s_copyz(msg, "hello world of possibilities!");

	s_cat_sha1hex(s, msg->s, msg->len);

	fwrite(msg->s, msg->len, 1, stdout);
	fputs(" = ", stdout);
	fwrite(s->s, s->len, 1, stdout);
	fputs("\n", stdout);

	assert(s_diffz(s, "a0546faeff640257907540e2901f5fdcbc8a90ef") == 0);

	s_free(msg);
	s_free(s);

	puts("ok");

	return 0;
}
