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
#include "higherc/s.h"

struct i {
	struct i *tail;
	struct hcns(s) tag;
};

#define NEW_I(p) do {struct i *__tmp = p; HC_NEW(p, struct i); p->tail = __tmp;} while (0)
#define FREE_I(p) do {hcns(s_free)(&p->tag); HC_FREE(p);} while (0)

int main(int argc, char **argv)
{
	int i;
	struct i *h = NULL, *h0;
	int h_len = 0;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		NEW_I(h);
		h_len++;

		hcns(s_copyz)(&h->tag, argv[i]);
	}
	h0 = h;

	if (h0 == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	/* reverse
	 */

	struct i **r;
	HC_NEW_AR(r, h_len, struct i*);

	{
		int l=h_len;
		for (h=h0; h; h=h->tail) {
			r[--l] = h;
		}
	}

	/* original order
	 */

	for (i=0; i<h_len; i++) {
		h = r[i];

		fwrite("[", 1, 1, stdout);
		fwrite(h->tag.s, h->tag.len, 1, stdout);
		fwrite("]\n", 2, 1, stdout);
	}

	/* cleanup
	 */

	for (h=h0; h;) {
		struct i *h_tail = h->tail;
		FREE_I(h);
		h = h_tail;
	}
	HC_FREE(r);

	return 0;
}
