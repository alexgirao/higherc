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
#include "higherc/pbuf.h"

struct mydata {
	int alpha;
	double bravo;
	char charlie[100];
};

int main(int argc, char **argv)
{
	int len = 10;
	int itemsiz = sizeof(struct mydata);
	int i, j;
	HC_DEF_PBUF(pbuf);

	hcns(pbuf_alloc)(pbuf, len, itemsiz);

	j = 2;
	while (j--) {
		/* enqueue up to len
		 */

		printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf->next, pbuf->enqueued, pbuf->list->length,
			hcns(pbuf_remaining_len)(pbuf));

		for (i=0; i<len; i++) {
			assert(pbuf->next == i);
			void *p = hcns(pbuf_enqueue)(pbuf);
			printf("enqueued %p, remaining %i, next %i\n", p, hcns(pbuf_remaining_len)(pbuf), pbuf->next);
		}

		printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf->next, pbuf->enqueued, pbuf->list->length,
			hcns(pbuf_remaining_len)(pbuf));

		assert(hcns(pbuf_remaining_len)(pbuf) == 0);

		/* dequeue up to len
		 */

		for (i=0; i<len; i++) {
			assert(hcns(pbuf_remaining_len)(pbuf) == i);
			void *p = hcns(pbuf_dequeue)(pbuf);
			printf("dequeued %p, remaining_len %i, next %i\n", p, hcns(pbuf_remaining_len)(pbuf), pbuf->next);
		}

		assert(hcns(pbuf_remaining_len)(pbuf) == len);
	}

	/*
	 */

	assert(len >= 5);

	j = 5;
	while (j--) {
		/* enqueue up to len
		 */

		printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf->next, pbuf->enqueued, pbuf->list->length,
			hcns(pbuf_remaining_len)(pbuf));

		for (i=0; i<3; i++) {
			void *p = hcns(pbuf_enqueue)(pbuf);
			printf("enqueued %p, remaining_len %i, next %i\n", p, hcns(pbuf_remaining_len)(pbuf), pbuf->next);
		}

		printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf->next, pbuf->enqueued, pbuf->list->length,
			hcns(pbuf_remaining_len)(pbuf));

		/* dequeue up to len
		 */

		for (i=0; i<3; i++) {
			void *p = hcns(pbuf_dequeue)(pbuf);
			printf("dequeued %p, remaining_len %i, next %i\n", p, hcns(pbuf_remaining_len)(pbuf), pbuf->next);
		}

		if (j == 0) {
			printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf->next, pbuf->enqueued, pbuf->list->length, hcns(pbuf_remaining_len)(pbuf));
		}
	}

	hcns(pbuf_free)(pbuf);

	return 0;
}
