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
#include "higherc/s.h"

/*
 */

HC_DECL_I(i,
	struct hcns(s) tag[1];
	int a;
	int b;
	int c;
);

/* I_FREE(p): calling bzero is a good practice, since all data in
 *   structure will be lost, we can't rely on buggy code accessing
 *   phantom data, it can be removed after extensive tests in
 *   production systems for performance reasons
 */

int main(int argc, char **argv)
{
	int i;
	struct i *h = NULL;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		h = i_new(h);
		hcns(s_copyz)(h->tag, argv[i]);
		HC_SAFE_CSTR(h->tag);
		h->a = i;
		h->b = h->tag->len;
		h->c = h->a * 1000 + h->b;
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	/* reverse
	 */

	struct i **r = i_as_array(h);

	/* traverse in original order
	 */

	for (i=0; i<=h->pos; i++) {
		struct i *tmp = r[i];
		fprintf(stdout, "[%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

        /* cleanup
	 */

	{
		struct i *t0=h;
		while (t0) {
			struct i *t1 = t0->tail;
			hcns(s_free)(t0->tag);
			i_free(t0);
			t0 = t1;
		}
		HC_FREE(r);
	}

	return 0;
}
