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
#include "higherc/bytewise.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"
#include "higherc/str.h"
#include "higherc/s.h"
#include "higherc/n.h"
#include "higherc/sha1.h"
#include "higherc/crc32.h"
#include "higherc/tagid.h"

/* see ~/projects/tagdb/README2
 */

static void print_s(char *prefix, HC_ST_S *s, char *suffix)
{
	if (prefix) printf("%s", prefix);
	fwrite(s->s, s->len, 1, stdout);
	if (suffix) printf("%s", suffix);
}

static void test0()
{
	HC_DEF_S(tid);
	HC_DEF_TAGID(tagid);
	HC_ST_TAG *h = NULL;
	char *check = "tid04079a5157de2jeg2ovjrtl0hfk79vsjjfva7prvj24";

	h = hcns(tag_newz)(h, "a");
	h = hcns(tag_newz)(h, "b");
	h = hcns(tag_newz)(h, "c");
	h = hcns(tag_newz)(h, "d");

	hcns(tagid_set_tags)(tagid, h);
	hcns(tagid_cat_id)(tagid, tid);

	assert(hcns(s_diffz)(tid, check) == 0);

	hcns(s_free)(tid);
	hcns(tag_free)(h);
	hcns(tagid_free)(tagid);
}

int main(int argc, char **argv)
{
	int i;
	HC_ST_TAG *h = NULL;
	HC_DEF_TAGID(tagid);
	HC_DEF_TAGID(a);
	unsigned char b[HC_TAGID_RAW_LEN];
	HC_DEF_S(A);
	HC_DEF_S(tid);

	for (i=1; i<argc; i++) {
		h = hcns(tag_newz)(h, argv[i]);
		if (h == NULL) {
			HC_FATAL("invalid tag at arg %i", i);
		}
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	test0();

	/*
	 */

	hcns(tagid_set_tags0)(tagid, A, h);
	hcns(tagid_cat_id)(tagid, tid);

	print_s(NULL, A, " = ");
	print_s(NULL, tid, "\n");

	/* serialization test
	 */

	hcns(tagid_put)(tagid, b);
	hcns(tagid_get)(a, b);

	hcns(tagid_cat_id)(tagid, tid);

	assert(hcns(b_diff)(tagid, sizeof(HC_ST_TAGID), a) == 0);

        /* cleanup
	 */

	hcns(s_free)(A);
	hcns(s_free)(tid);
	hcns(tag_free)(h);
	hcns(tagid_free)(tagid);

	return 0;
}
