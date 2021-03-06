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
	int pos;
	struct i *tail;
	struct s tag;
};

#define I_NEW(p) do {struct i *__tmp = p; HC_NEW(p, struct i); p->pos = __tmp ? __tmp->pos + 1 : 0; p->tail = __tmp;} while (0)
#define I_FREE(p) do {s_free(&p->tag); bzero(p, sizeof(struct i)); HC_FREE(p);} while (0)
#define I_ARRAY(r, h)  do {					\
		HC_NEW_AR(r, h->pos + 1, struct i*);		\
		{						\
			int __i=0; struct i *__tmp=h;		\
			for (; __tmp; __tmp=__tmp->tail) {	\
				r[__i++] = __tmp;		\
			}					\
		}						\
	} while (0)
#define I_REVERSED(r, h) do {					\
		HC_NEW_AR(r, h->pos + 1, struct i*);		\
		{						\
			struct i *__tmp=h;			\
			for (; __tmp; __tmp=__tmp->tail) {	\
				r[__tmp->pos] = __tmp;		\
			}					\
		}						\
	} while (0)

/* I_FREE(p): calling bzero is a good practice, since all data in
 *   structure will be lost, we can't rely on buggy code accessing
 *   phantom data, it can be removed after extensive tests in
 *   production systems for performance reasons
 */

int main(int argc, char **argv)
{
	int i;
	struct i *h = NULL;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		I_NEW(h);
		s_copyz(&h->tag, argv[i]);
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	/* reverse
	 */

	struct i **r;
	I_REVERSED(r, h);

	/* traverse in original order
	 */

	for (i=0; i<=h->pos; i++) {
		struct i *tmp = r[i];

		fwrite("[", 1, 1, stdout);
		fwrite(tmp->tag.s, tmp->tag.len, 1, stdout);
		fwrite("]\n", 2, 1, stdout);
	}

        /* cleanup
	 */

	{
		struct i *t0=h;
		while (t0) {
			struct i *t1 = t0->tail;
			I_FREE(t0);
			t0 = t1;
		}
		HC_FREE(r);
	}

	return 0;
}
