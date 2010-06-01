#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"
#include "higherc/s.h"

/*
 */

HC_DECL_PRIVATE_I(i,
	struct hcns(s) tag[1];
	int a;
	int b;
	int c;
);

int main(int argc, char **argv)
{
	int i, j;
	struct i *h = NULL, *t;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		h = i_new0(h);
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

	struct i_iter c[1];

	/* backward traverse
	 */

	fprintf(stdout, "backward traverse\n");

	i_backward(h, c);
	while ((t = i_next(c))) {
		fprintf(stdout, "  [%s][%i][%i][%i]\n", t->tag->s, t->a, t->b, t->c);
	}
	i_end(c);

	/* forward traverse
	 */

	fprintf(stdout, "forward traverse\n");

	i_forward(h, c);
	while ((t = i_next(c))) {
		fprintf(stdout, "  [%s][%i][%i][%i]\n", t->tag->s, t->a, t->b, t->c);
	}
	i_end(c);

	/* forward traverse, from array
	 */

	struct i **r = i_as_array(h);

	fprintf(stdout, "forward traverse (from array)\n");

	for (i=0, j=i_len(h); i<j; i++) {
		struct i *tmp = r[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

        /* cleanup
	 */

	HC_FREE(r);

	i_backward(h, c);
	while ((t = i_next(c))) {
		hcns(s_free)(t->tag);
	}
	i_end(c);

	i_free0(h);

	return 0;
}
