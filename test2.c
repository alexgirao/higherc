#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"

struct mydata {
	int alpha;
	double bravo;
	char charlie[100];
};

int main(int argc, char **argv)
{
	int len = 10;
	struct hcns(list) *list = hcns(list_alloc)(len, len * sizeof(struct mydata), NULL);
	int i;

	for (i=0; i<list->length; i++) {
		int sz = sizeof(struct mydata);
		int sz2;
		void *p = hcns(item_setup)(list, i, sz);
		void *p2 = hcns(item_get)(list, i, &sz2);

		assert(p == p2);
		assert(sz == sz2);

		struct mydata *ms = p;
		ms->alpha = i;
		ms->bravo = i * 100.0;
		snprintf(ms->charlie, sz, "item %i is at %p", i, p);

		printf("requested %p - %p\n", p, higherc_offset(p, sz));
	}

	for (i=0; i<list->length; i++) {
		int sz;
		struct mydata *ms = hcns(item_get)(list, i, &sz);
		printf("{%i, %.2f, %s}\n", ms->alpha, ms->bravo, ms->charlie);
	}

	hcns(list_free)(list);

	return 0;
}
