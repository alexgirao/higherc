#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"

#define HIGHERC_LIST_DEBUG

static void fatal(char *file, int line, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	fprintf(stderr, "fatal-error: %s: %i: ", file, (int)line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	exit(-1);
}

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define FATAL(...)       fatal(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define FATAL(fmt...)   fatal(__FILE__, __LINE__, fmt)
#endif

struct hcns(list)* hcns(list_alloc)(int length, int bufsz, struct hcns(list) *tail)
{
	int allocsz0 = sizeof(struct hcns(list)) + length * sizeof(struct hcns(item));
	int allocsz = allocsz0 + bufsz;

	struct hcns(list) *r = calloc(1, allocsz);
	if (r == NULL) {
		FATAL("calloc(1, %i)", allocsz);
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

void hcns(list_free)(struct hcns(list) *list)
{
	if (list->refcnt <= 0) {
		FATAL("list_free(%p) with refcnt = %i", list, list->refcnt);
	}
	if (--list->refcnt == 0) {
#ifdef HIGHERC_LIST_DEBUG
		printf("list_free(%p), 0 references, freeing\n", list);
#endif
		free(list);
	}
#ifdef HIGHERC_LIST_DEBUG
	else {
		printf("list_free(%p), new refcnt %i\n", list, list->refcnt);
	}
#endif
}

void *hcns(item_setup)(struct hcns(list) *list, int index, int size)
{
	struct hcns(item) *first_item = (struct hcns(item) *) higherc_offset(list, sizeof(struct hcns(list)));
	struct hcns(item) *item = first_item + index;

	int allocsz0 = sizeof(struct hcns(list)) + list->length * sizeof(struct hcns(item));

	if (item->pos != 0) {
		FATAL("item_setup(%p, %i, %i), called multiple times", list, index, size);
	}

	if (list->lastpos + size > allocsz0 + list->bufsiz) {
		FATAL("item_setup(%p, %i, %i), buffer overflow attempt detected", list, index, size);
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

	if (item->pos == 0) {
		return NULL;
	}

	if (sizep) {
		*sizep = item->size;
	}

	return higherc_offset(list, item->pos);
}
