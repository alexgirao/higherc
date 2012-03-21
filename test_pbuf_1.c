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
	int i, j, pos;
	struct mydata *p;
	HC_DEF_PBUF(pbuf);

	pbuf_alloc(pbuf, len, itemsiz);

	for (i=0; i<len; i++) {
		p = pbuf_enqueue(pbuf);
		p->alpha = i;
	}

	assert(len == 10);

	assert(p == pbuf_shift(pbuf));
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == pbuf->enqueued);

	assert(pbuf->enqueued == 0);
	assert(pbuf_shift(pbuf) == NULL);

	/* fillup again, up to 7
	 */

	for (i=0; i<7; i++) {
		p = pbuf_enqueue(pbuf);
		p->alpha = i * 100;
	}

	assert(((struct mydata *)pbuf_dequeue(pbuf))->alpha == 0);
	assert(((struct mydata *)pbuf_dequeue(pbuf))->alpha == 100);
	assert(((struct mydata *)pbuf_shift(pbuf))->alpha == 600);

	/* traverse queued slots
	 */

	i = 0;
	j = pbuf_enqueued_len(pbuf);
	pos = pbuf_enqueued_first(pbuf);
	for (; i<j; i++) {
		p = pbuf_item(pbuf, pos);
		pos = pbuf_next(pbuf, pos);
		printf("enqueued[%i] = %i\n", pos, p->alpha);
	}

	/* traverse remaining items
	 */

	i = 0;
	j = pbuf_remaining_len(pbuf);
	pos = pbuf_remaining_first(pbuf);
	for (; i<j; i++) {
		p = pbuf_item(pbuf, pos);
		pos = pbuf_next(pbuf, pos);
		printf("remaining[%i] = %i\n", pos, p->alpha);
	}

	/* traverse queued slots (backward)
	 */

	i = 0;
	j = pbuf_enqueued_len(pbuf);
	pos = pbuf_enqueued_last(pbuf);
	for (; i<j; i++) {
		p = pbuf_item(pbuf, pos);
		pos = pbuf_prior(pbuf, pos);
		printf("enqueued[%i] = %i\n", pos, p->alpha);
	}

	/* traverse remaining items (backward)
	 */

	i = 0;
	j = pbuf_remaining_len(pbuf);
	pos = pbuf_remaining_last(pbuf);
	for (; i<j; i++) {
		p = pbuf_item(pbuf, pos);
		pos = pbuf_prior(pbuf, pos);
		printf("remaining[%i] = %i\n", pos, p->alpha);
	}

	pbuf_free(pbuf);

	return 0;
}
