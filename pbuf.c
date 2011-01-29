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

void hcns(pbuf_alloc)(struct hcns(pbuf) *pbuf, int len, int itemsiz)
{
	int i;
	struct hcns(list) *list;

	assert(pbuf->list == NULL);
	assert(len > 0);

	list = hcns(list_alloc)(len, len * itemsiz, NULL);

	/* setup items
	 */

	for (i=0; i<len; i++) {
		void *p = hcns(item_setup)(list, i, itemsiz);
		if (p == NULL) {
			hcns(list_free)(list);
			list = NULL;
			return;
		}
	}

	pbuf->list = list;
	pbuf->itemsiz = itemsiz;
	pbuf->next = 0;
	pbuf->enqueued = 0;
}

void hcns(pbuf_free)(struct hcns(pbuf) *pbuf)
{
	if (pbuf->list) {
		struct hcns(list) *l = pbuf->list;
		pbuf->list = NULL;
		pbuf->itemsiz = 0;
		hcns(list_free)(l);
	}
}

void *hcns(pbuf_enqueue)(struct hcns(pbuf) *pbuf)
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

	return hcns(item_get)(pbuf->list, pos, NULL);
}

void *hcns(pbuf_shift)(struct hcns(pbuf) *pbuf)
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

	return hcns(item_get)(pbuf->list, pbuf->next, NULL);
}

void *hcns(pbuf_dequeue)(struct hcns(pbuf) *pbuf)
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

	return hcns(item_get)(pbuf->list, pos, NULL);
}

int hcns(pbuf_enqueued_len)(struct hcns(pbuf) *pbuf)
{
	return pbuf->enqueued;
}

int hcns(pbuf_enqueued_first)(struct hcns(pbuf) *pbuf)
{
	int pos = pbuf->next - pbuf->enqueued;
	if (pos < 0) {
		/* two's complement of queue length
		 */
		pos = pbuf->list->length - -pos;
	}
	return pos;
}

int hcns(pbuf_enqueued_last)(struct hcns(pbuf) *pbuf)
{
	return (pbuf->next + pbuf->list->length - 1) % pbuf->list->length;
}

int hcns(pbuf_remaining_len)(struct hcns(pbuf) *pbuf)
{
	return pbuf->list->length - pbuf->enqueued;
}

int hcns(pbuf_remaining_first)(struct hcns(pbuf) *pbuf)
{
	return pbuf->next;
}

int hcns(pbuf_remaining_last)(struct hcns(pbuf) *pbuf)
{
	return (hcns(pbuf_enqueued_first)(pbuf) + pbuf->list->length - 1) % pbuf->list->length;
}

int hcns(pbuf_len)(struct hcns(pbuf) *pbuf)
{
	return pbuf->list->length;
}

int hcns(pbuf_next)(struct hcns(pbuf) *pbuf, int pos)
{
	return (pos + 1) % pbuf->list->length;
}

int hcns(pbuf_prior)(struct hcns(pbuf) *pbuf, int pos)
{
	return (pos + pbuf->list->length - 1) % pbuf->list->length;
}

void *hcns(pbuf_item)(struct hcns(pbuf) *pbuf, int pos)
{
	assert(pos >= 0);
	return hcns(item_get)(pbuf->list, pos % pbuf->list->length, NULL);
}
