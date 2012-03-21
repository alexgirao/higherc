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
#include "higherc/alloc.h"
#include "higherc/list.h"
#include "higherc/pbuf.h"

void pbuf_alloc(struct pbuf *pbuf, int len, int itemsiz)
{
	int i;
	struct list *list;

	assert(pbuf->list == NULL);
	assert(len > 0);

	list = list_alloc(len, len * itemsiz, NULL);

	/* setup items
	 */

	for (i=0; i<len; i++) {
		void *p = item_setup(list, i, itemsiz);
		if (p == NULL) {
			list_free(list);
			list = NULL;
			return;
		}
	}

	pbuf->list = list;
	pbuf->itemsiz = itemsiz;
	pbuf->next = 0;
	pbuf->enqueued = 0;
}

void pbuf_free(struct pbuf *pbuf)
{
	if (pbuf->list) {
		struct list *l = pbuf->list;
		pbuf->list = NULL;
		pbuf->itemsiz = 0;
		list_free(l);
	}
}

void *pbuf_enqueue(struct pbuf *pbuf)
{
	int pos = pbuf->next;

	assert(pos >= 0);
	assert(pos < pbuf->list->length);

	assert(pbuf->enqueued >= 0);
	assert(pbuf->enqueued <= pbuf->list->length);

	if (pbuf->enqueued == pbuf->list->length) {
		/* queue is full
		 */
		return NULL;
	}

	/* cycle
	 */
	pbuf->next = (pbuf->next + 1) % pbuf->list->length;
	pbuf->enqueued++;

	return item_get(pbuf->list, pos, NULL);
}

void *pbuf_shift(struct pbuf *pbuf)
{
	assert(pbuf->next >= 0);
	assert(pbuf->next < pbuf->list->length);

	assert(pbuf->enqueued >= 0);
	assert(pbuf->enqueued <= pbuf->list->length);

	if (pbuf->enqueued == 0) {
		/* queue is empty
		 */
		return NULL;
	}

	/* cycle (backward)
	 */
	pbuf->next = (pbuf->next + pbuf->list->length - 1) % pbuf->list->length;
	pbuf->enqueued--;

	return item_get(pbuf->list, pbuf->next, NULL);
}

void *pbuf_dequeue(struct pbuf *pbuf)
{
	int pos;

	assert(pbuf->enqueued >= 0);

	if (pbuf->enqueued == 0) {
		/* queue is empty
		 */
		return NULL;
	}

	pos = pbuf->next - pbuf->enqueued;
	if (pos < 0) {
		/* two's complement of queue length
		 */
		pos = pbuf->list->length - -pos;
	}
	pbuf->enqueued--;

	return item_get(pbuf->list, pos, NULL);
}

int pbuf_enqueued_len(struct pbuf *pbuf)
{
	return pbuf->enqueued;
}

int pbuf_enqueued_first(struct pbuf *pbuf)
{
	int pos = pbuf->next - pbuf->enqueued;
	if (pos < 0) {
		/* two's complement of queue length
		 */
		pos = pbuf->list->length - -pos;
	}
	return pos;
}

int pbuf_enqueued_last(struct pbuf *pbuf)
{
	return (pbuf->next + pbuf->list->length - 1) % pbuf->list->length;
}

int pbuf_remaining_len(struct pbuf *pbuf)
{
	return pbuf->list->length - pbuf->enqueued;
}

int pbuf_remaining_first(struct pbuf *pbuf)
{
	return pbuf->next;
}

int pbuf_remaining_last(struct pbuf *pbuf)
{
	return (pbuf_enqueued_first(pbuf) + pbuf->list->length - 1) % pbuf->list->length;
}

int pbuf_len(struct pbuf *pbuf)
{
	return pbuf->list->length;
}

int pbuf_next(struct pbuf *pbuf, int pos)
{
	return (pos + 1) % pbuf->list->length;
}

int pbuf_prior(struct pbuf *pbuf, int pos)
{
	return (pos + pbuf->list->length - 1) % pbuf->list->length;
}

void *pbuf_item(struct pbuf *pbuf, int pos)
{
	assert(pos >= 0);
	return item_get(pbuf->list, pos % pbuf->list->length, NULL);
}
