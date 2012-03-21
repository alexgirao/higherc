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

/*

./test-4 alpha bravo charlie delta alpha bravo
./test-4 alpha bravo charlie delta alpha charlie
./test-4 alpha bravo charlie delta alpha delta
./test-4 alpha bravo charlie delta bravo alpha
./test-4 alpha bravo charlie delta bravo charlie
./test-4 alpha bravo charlie delta bravo delta


 */

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

#define AA_CMPEXPR s_diff(x->tag, y->tag)

#ifndef TEST_PRIVATE
#include "test_4.h"
#else
HC_DECL_PRIVATE_I(aa,
	struct s tag[1];
	int a;
	int b;
	int c;
);
HC_DECL_PRIVATE_I_SORT(aa, x, y, AA_CMPEXPR);
HC_DECL_PRIVATE_I_USORT(aa, x, y, AA_CMPEXPR);
#endif

static struct aa *aa_new(struct aa *h, int argc, char *arg)
{
	h = aa_new0(h);

	s_copyz(h->tag, arg);
	HC_SAFE_CSTR(h->tag);

	h->a = argc;
	h->b = h->tag->len;
	h->c = h->a * 1000 + h->b;

	return h;
}

static void aa_free(struct aa *h)
{
	struct aa *t;
	struct aa_iter c[1];
	aa_backward(c, h);
	while ((t = aa_next(c))) {
		s_free(t->tag);
	}
	aa_end(c);
	aa_free0(h);
}

void traverse_post_order(struct aa *h)
{
	if (h) {
		traverse_post_order(h->tail);
		fprintf(stdout, "  [%s][%i][%i][%i]\n", h->tag->s, h->a, h->b, h->c);
	}
}

int main(int argc, char **argv)
{
	int i, j;
	struct aa *h = NULL;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		h = aa_new(h, i, argv[i]);
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	/* sorted traverse
	 */

	struct aa **sorted = aa_as_array(h);
	aa_sort(sorted, aa_len(h));

	fprintf(stdout, "sorted traverse (from sorted array)\n");

	for (i=0, j=aa_len(h); i<j; i++) {
		struct aa *tmp = sorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	/* .. descending order
	 */

	aa_sort_desc(sorted, aa_len(h));

	fprintf(stdout, "sorted traverse (from sorted array, descending order)\n");

	for (i=0, j=aa_len(h); i<j; i++) {
		struct aa *tmp = sorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	HC_FREE(sorted);

	/* sorted traverse
	 */

	struct aa **usorted = aa_as_array(h);
	int newlen = aa_usort(usorted, aa_len(h));

	fprintf(stdout, "sorted traverse (from uniquely sorted array)\n");

	for (i=0, j=newlen; i<j; i++) {
		struct aa *tmp = usorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	HC_FREE(usorted);

	/* .. descending order
	 */

	usorted = aa_as_array(h);
	newlen = aa_usort_desc(usorted, aa_len(h));

	fprintf(stdout, "sorted traverse (from uniquely sorted array, descending order)\n");

	for (i=0, j=newlen; i<j; i++) {
		struct aa *tmp = usorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	/* post order traversal
	 */

	fprintf(stdout, "post order traversal (forward order)\n");

	traverse_post_order(h);

	/* foreach (backward order)
	 */

	fprintf(stdout, "foreach (backward order)\n");

	{
		struct aa *tmp2[2] = {0, h};
		while (aa_foreach(tmp2)) {
			struct aa *tmp = *tmp2;
			fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
		}
	}

	/* reverse
	 */

	h = aa_reverse(h);
	fprintf(stdout, "foreach after reverse (forward order)\n");

	{
		struct aa *tmp2[2] = {0, h};
		while (aa_foreach(tmp2)) {
			struct aa *tmp = *tmp2;
			fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
		}
	}

	/*
	 */

	HC_FREE(usorted);

	/* cleanup
	 */

	aa_free(h);

	return 0;
}

#ifndef TEST_PRIVATE
HC_DECL_PUBLIC_I(aa);
HC_DECL_PUBLIC_I_SORT(aa, x, y, AA_CMPEXPR);
HC_DECL_PUBLIC_I_USORT(aa, x, y, AA_CMPEXPR);
#endif
