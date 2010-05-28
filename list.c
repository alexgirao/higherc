#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"
#include "higherc/list.h"

/*#define HIGHERC_LIST_DEBUG*/

struct hcns(list)* hcns(list_alloc)(int length, int bufsz, struct hcns(list) *tail)
{
	int allocsz0 = sizeof(struct hcns(list)) + length * sizeof(struct hcns(item));
	int allocsz = allocsz0 + bufsz;
	struct hcns(list) *r = NULL;

	if (tail) {
		/* not implemented-yet, see TODO */
		return NULL;
	}

	HC_ALLOC_Z(r, allocsz);
	r->refcnt = 1;
	r->length = length;
	r->bufsiz = bufsz;
	r->lastpos = allocsz0;
	r->tail = tail;

#ifdef HIGHERC_LIST_DEBUG
	printf("list header + items: %p - %p\n", r, HC_OFFSET(r, allocsz0));
	printf("user data:           %p - %p\n", HC_OFFSET(r, allocsz0), HC_OFFSET(r, allocsz));
#endif

	return r;
}

void hcns(list_free)(struct hcns(list) *list)
{
	assert(list->refcnt > 0);

	if (--list->refcnt == 0) {
#ifdef HIGHERC_LIST_DEBUG
		printf("list_free(%p), 0 references, freeing\n", list);
#endif
		HC_FREE(list);
	}
#ifdef HIGHERC_LIST_DEBUG
	else {
		printf("list_free(%p), new refcnt %i\n", list, list->refcnt);
	}
#endif
}

void *hcns(item_setup)(struct hcns(list) *list, int index, int size)
{
	struct hcns(item) *first_item = (struct hcns(item) *) HC_OFFSET(list, sizeof(struct hcns(list)));
	struct hcns(item) *item = first_item + index;

	int allocsz0 = sizeof(struct hcns(list)) + list->length * sizeof(struct hcns(item));

	assert(index >= 0);

	if (item->pos != 0) {
		//FATAL("item_setup(%p, %i, %i), called multiple times", list, index, size);
		return NULL;
	}

	if (list->lastpos + size > allocsz0 + list->bufsiz) {
		//FATAL("item_setup(%p, %i, %i), buffer overflow attempt detected", list, index, size);
		return NULL;
	}

	item->pos = list->lastpos;
	item->size = size;

	list->lastpos = list->lastpos + size;

	return HC_OFFSET(list, item->pos);
}

void *hcns(item_get)(struct hcns(list) *list, int index, int *sizep)
{
	struct hcns(item) *first_item = (struct hcns(item) *) HC_OFFSET(list, sizeof(struct hcns(list)));
	struct hcns(item) *item = first_item + index;

	assert(index >= 0);

	if (item->pos == 0) {
		/* undefined */
		return NULL;
	}

	if (sizep) {
		*sizep = item->size;
	}

	return HC_OFFSET(list, item->pos);
}
