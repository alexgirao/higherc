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

/*#define HIGHERC_LIST_DEBUG*/

struct list* list_alloc(int length, int bufsz, struct list *tail)
{
	int allocsz0 = sizeof(struct list) + length * sizeof(struct item);
	int allocsz = allocsz0 + bufsz;
	struct list *r = NULL;

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

void list_free(struct list *list)
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

void *item_setup(struct list *list, int index, int size)
{
	struct item *first_item = (struct item *) HC_OFFSET(list, sizeof(struct list));
	struct item *item = first_item + index;

	int allocsz0 = sizeof(struct list) + list->length * sizeof(struct item);

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

void *item_get(struct list *list, int index, int *sizep)
{
	struct item *first_item = (struct item *) HC_OFFSET(list, sizeof(struct list));
	struct item *item = first_item + index;

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
