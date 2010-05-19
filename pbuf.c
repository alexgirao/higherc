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

hcns(bool) hcns(pbuf_alloc)(struct hcns(pbuf) *pbuf, int length, int itemsiz)
{
	struct hcns(list) *list;

	if (pbuf->list) {
		/* list must be initialized with HIGHERC_NULL_PBUF */
		return 0;
	}

	if (length < 1) {
		/* invalid length */
		return 0;
	}

	list = hcns(list_alloc)(length, length * itemsiz, NULL);
	if (list == NULL) {
		return 0;
	}

	/* setup items
	 */

	{
		int i;
		for (i=0; i<length; i++) {
			void *p = hcns(item_setup)(list, i, itemsiz);
			if (p == NULL) {
				hcns(list_free)(list);
				list = NULL;
				return 1;
			}
		}
	}

	pbuf->list = list;
	pbuf->itemsiz = itemsiz;
	pbuf->next = 0;
	pbuf->enqueued = 0;

	return 1;
}

hcns(bool) hcns(pbuf_free)(struct hcns(pbuf) *pbuf)
{
	int r = 1;
	if (pbuf->list) {
		struct hcns(list) *l = pbuf->list;
		pbuf->list = NULL;
		pbuf->itemsiz = 0;
		r = hcns(list_free)(l);
	}
	return r;
}

int hcns(remaining)(struct hcns(pbuf) *pbuf)
{
	return pbuf->list->length - pbuf->enqueued;
}

void *hcns(enqueue)(struct hcns(pbuf) *pbuf)
{
	int pos = pbuf->next;

	assert(pos >= 0);
	assert(pos < pbuf->list->length);
	assert(pbuf->enqueued <= pbuf->list->length);

	if (pbuf->enqueued == pbuf->list->length) {
		/* queue is full */
		return NULL;
	}

	/* cycle */
	pbuf->next = (pbuf->next + 1) % pbuf->list->length;
	pbuf->enqueued++;

	return hcns(item_get)(pbuf->list, pos, NULL);
}

void *hcns(dequeue)(struct hcns(pbuf) *pbuf)
{
	int pos;

	assert(pbuf->enqueued >= 0);

	if (pbuf->enqueued == 0) {
		/* queue is empty */
		return NULL;
	}

	pos = pbuf->next - pbuf->enqueued;
	if (pos < 0) {
		/* two's complement of queue length */
		pos = pbuf->list->length - -pos;
	}
	pbuf->enqueued--;

	return hcns(item_get)(pbuf->list, pos, NULL);
}
