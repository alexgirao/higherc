#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc.h"
#include "buffer.h"
#include "list.h"

struct hcns(list)* hcns(list_alloc)(int length, int bufsz, struct hcns(list) *tail)
{
	struct hcns(list) *r = calloc(1, sizeof(struct hcns(list)) + length*sizeof(struct hcns(item))  + bufsz);
	r->length = length;
	r->tail = tail;
	return r;
}

void hcns(list_free)(struct hcns(list) *list)
{
	free(list);
}
