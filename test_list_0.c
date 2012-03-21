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
#include <stdarg.h>

#include "higherc/higherc.h"
#include "higherc/list.h"

struct mydata {
	int alpha;
	double bravo;
	char charlie[100];
};

int main(int argc, char **argv)
{
	int len = 10;
	int bufsiz = len * sizeof(struct mydata);
	struct list *list = list_alloc(len, bufsiz, NULL);
	int i;

	if (!list) {
		HC_FATAL("failed to create a list with %i items and %i bytes buffer", len, bufsiz);
	}

	for (i=0; i<list->length; i++) {
		int sz = sizeof(struct mydata);
		int sz2;
		void *p = item_setup(list, i, sz);
		void *p2 = item_get(list, i, &sz2);

		assert(p == p2);
		assert(sz == sz2);

		struct mydata *ms = p;
		ms->alpha = i;
		ms->bravo = i * 100.0;
		snprintf(ms->charlie, sz, "item %i is at %p", i, p);

		printf("requested %p - %p\n", p, HC_OFFSET(p, sz));
	}

	for (i=0; i<list->length; i++) {
		int sz;
		struct mydata *ms = item_get(list, i, &sz);
		printf("{%i, %.2f, %s}\n", ms->alpha, ms->bravo, ms->charlie);
	}

	list_free(list);

	return 0;
}
