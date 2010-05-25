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

int hcns(remaining)(struct hcns(pbuf) *pbuf)
{
	assert(pbuf->enqueued >= 0);
	return pbuf->list->length - pbuf->enqueued;
}

void *hcns(enqueue)(struct hcns(pbuf) *pbuf)
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

void *hcns(shift)(struct hcns(pbuf) *pbuf)
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

void *hcns(dequeue)(struct hcns(pbuf) *pbuf)
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
		/* two's complement of queue length */
		pos = pbuf->list->length - -pos;
	}
	pbuf->enqueued--;

	return hcns(item_get)(pbuf->list, pos, NULL);
}
