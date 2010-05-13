#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/readfd.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"
#include "higherc/fatal.h"
#include "higherc/stralloc.h"

struct i {
	int pos;
	struct i *tail;
	struct hcns(s) tag;
};

#define NEW_I(p) do {struct i *__tmp = p; HC_NEW(p, struct i); p->pos = __tmp ? __tmp->pos + 1 : 0; p->tail = __tmp;} while (0)
#define FREE_I(p) do {hcns(s_free)(&p->tag); hcns(bzero)(p, sizeof(struct i)); HC_FREE(p);} while (0)

int main(int argc, char **argv)
{
	int i;
	struct i *h = NULL;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		NEW_I(h);
		hcns(s_copyz)(&h->tag, argv[i]);
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	/* reverse
	 */

	struct i **r;
	HC_NEW_AR(r, h->pos + 1, struct i*);

	{
		struct i *tmp=h;
		for (; tmp; tmp=tmp->tail) {
			r[tmp->pos] = tmp;
		}
	}

	/* original order
	 */

	for (i=0; i<=h->pos; i++) {
		struct i *tmp = r[i];

		fwrite("[", 1, 1, stdout);
		fwrite(tmp->tag.s, tmp->tag.len, 1, stdout);
		fwrite("]\n", 2, 1, stdout);
	}

/* cleanup
 */

	{
		struct i *t0=h;
		for (; t0; t0=t0->tail) {
			struct i *t1 = t0->tail;
			FREE_I(t0);
			t0 = t1;
		}
		HC_FREE(r);
	}

	return 0;
}
