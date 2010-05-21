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
#include "higherc/s.h"

/*
 */

HC_DECL_I(i,
	struct hcns(s) tag[1];
	int a;
	int b;
	int c;
);

static int i_len(struct i *x)
{
	return x->pos + 1;
}

static inline struct i *_i_next_b(struct hcns(iter) *i)
{
	struct i *r = i->v0;
	if (r == NULL) {
		return NULL;
	}
	i->v0 = r->tail;
	return r;
}

static void i_backward(struct i *x, struct hcns(iter) *i)
{
	i->next = (hcns(iter_next_fun)*) _i_next_b;

	i->i0 = 1;
	i->v0 = x;
}

static inline struct i *_i_next_f(struct hcns(iter) *i)
{
	struct i **r = i->v1;
	if (r == NULL) {
		return NULL;
	}
	if (i->l0-- == 0) {
		HC_FREE(i->v0);
		i->v0 = NULL;
		i->v1 = NULL;
		return NULL;
	}
	i->v1 = r + 1;
	return *r;
}

static void i_forward(struct i *x, struct hcns(iter) *i)
{
	i->next = (hcns(iter_next_fun)*) _i_next_f;

	i->i0 = 2;
	i->v0 = i_as_array(x);
	i->v1 = i->v0;
	i->l0 = x->pos + 1;
}

static void i_end(struct hcns(iter) *i)
{
	if (i->i0 == 2) {
		if (i->v0) {
			printf("free: %p\n", i->v0);
			HC_FREE(i->v0);
			i->v0 = NULL;
		}
	} else if (i->i0 == 1) {
	} else {
		HC_FATAL("exhausted");
	}
}

int main(int argc, char **argv)
{
	int i, j;
	struct i *h = NULL, *t;

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

	/* to array
	 */

	struct hcns(iter) c[1];

	/* backward traverse
	 */

	fprintf(stdout, "backward traverse\n");

	i_backward(h, c);
	while ((t = c->next(c))) {
		fprintf(stdout, "  [%s][%i][%i][%i]\n", t->tag->s, t->a, t->b, t->c);
	}
	i_end(c);

	/* forward traverse
	 */

	fprintf(stdout, "forward traverse\n");

	i_forward(h, c);
	while ((t = c->next(c))) {
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

	i_backward(h, c);
	while ((t = c->next(c))) {
		hcns(s_free)(t->tag);
		i_free(t);
	}
	i_end(c);

	HC_FREE(r);

	return 0;
}
