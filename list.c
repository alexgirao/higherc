#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"

#define HIGHERC_LIST_DEBUG

struct hcns(list)* hcns(list_alloc)(int length, int bufsz, struct hcns(list) *tail)
{
	int allocsz0 = sizeof(struct hcns(list)) + length * sizeof(struct hcns(item));
	int allocsz = allocsz0 + bufsz;

	struct hcns(list) *r = hcns(alloc_z)(allocsz);
	if (!r) {
		return NULL;
	}

	r->refcnt = 1;
	r->length = length;
	r->bufsiz = bufsz;
	r->lastpos = allocsz0;
	r->tail = tail;

#ifdef HIGHERC_LIST_DEBUG
	printf("list header + items: %p - %p\n", r, higherc_offset(r, allocsz0));
	printf("user data:           %p - %p\n", higherc_offset(r, allocsz0), higherc_offset(r, allocsz));
#endif

	return r;
}

hcns(bool) hcns(list_free)(struct hcns(list) *list)
{
	if (list->refcnt <= 0) {
		return 0;
	}
	if (--list->refcnt == 0) {
#ifdef HIGHERC_LIST_DEBUG
		printf("list_free(%p), 0 references, freeing\n", list);
#endif
		hcns(alloc_free)((void *)list);
	}
#ifdef HIGHERC_LIST_DEBUG
	else {
		printf("list_free(%p), new refcnt %i\n", list, list->refcnt);
	}
#endif
	return 1;
}

void *hcns(item_setup)(struct hcns(list) *list, int index, int size)
{
	struct hcns(item) *first_item = (struct hcns(item) *) higherc_offset(list, sizeof(struct hcns(list)));
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

	return higherc_offset(list, item->pos);
}

void *hcns(item_get)(struct hcns(list) *list, int index, int *sizep)
{
	struct hcns(item) *first_item = (struct hcns(item) *) higherc_offset(list, sizeof(struct hcns(list)));
	struct hcns(item) *item = first_item + index;

	assert(index >= 0);

	if (item->pos == 0) {
		/* undefined */
		return NULL;
	}

	if (sizep) {
		*sizep = item->size;
	}

	return higherc_offset(list, item->pos);
}
