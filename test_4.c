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

#include "test_4.h"

HC_DECL_PUBLIC_I(hcns(aa));

#define AA_CMPEXPR hcns(s_diff)(x->tag, y->tag)

HC_DECL_PUBLIC_I_SORT(hcns(aa), x, y, AA_CMPEXPR);
HC_DECL_PUBLIC_I_USORT(hcns(aa), x, y, AA_CMPEXPR);

static struct hcns(aa) *aa_new(struct hcns(aa) *h, int argc, char *arg)
{
	h = hcns(aa_new0)(h);

	hcns(s_copyz)(h->tag, arg);
	HC_SAFE_CSTR(h->tag);

	h->a = argc;
	h->b = h->tag->len;
	h->c = h->a * 1000 + h->b;

	return h;
}

static void aa_free(struct hcns(aa) *h)
{
	struct hcns(aa) *t;
	struct hcns(aa_iter) c[1];
	hcns(aa_backward)(c, h);
	while ((t = hcns(aa_next)(c))) {
		hcns(s_free)(t->tag);
	}
	hcns(aa_end)(c);
	hcns(aa_free0)(h);
}

int main(int argc, char **argv)
{
	int i, j;
	struct hcns(aa) *h = NULL;

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

	struct hcns(aa) **sorted = hcns(aa_as_array)(h);
	hcns(aa_sort)(sorted, hcns(aa_len)(h));

	fprintf(stdout, "sorted traverse (from sorted array)\n");

	for (i=0, j=hcns(aa_len)(h); i<j; i++) {
		struct hcns(aa) *tmp = sorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	/* .. descending order
	 */

	hcns(aa_sort_desc)(sorted, hcns(aa_len)(h));

	fprintf(stdout, "sorted traverse (from sorted array, descending order)\n");

	for (i=0, j=hcns(aa_len)(h); i<j; i++) {
		struct hcns(aa) *tmp = sorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	HC_FREE(sorted);

	/* sorted traverse
	 */

	struct hcns(aa) **usorted = hcns(aa_as_array)(h);
	int newlen = hcns(aa_usort)(usorted, hcns(aa_len)(h));

	fprintf(stdout, "sorted traverse (from uniquely sorted array)\n");

	for (i=0, j=newlen; i<j; i++) {
		struct hcns(aa) *tmp = usorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	HC_FREE(usorted);

	/* .. descending order
	 */

	usorted = hcns(aa_as_array)(h);
	newlen = hcns(aa_usort_desc)(usorted, hcns(aa_len)(h));

	fprintf(stdout, "sorted traverse (from uniquely sorted array, descending order)\n");

	for (i=0, j=newlen; i<j; i++) {
		struct hcns(aa) *tmp = usorted[i];
		fprintf(stdout, "  [%s][%i][%i][%i]\n", tmp->tag->s, tmp->a, tmp->b, tmp->c);
	}

	/* foreach (unspecified order)
	 */

	fprintf(stdout, "foreach (unspecified order)\n");

	{
		struct hcns(aa) *tmp2[2] = {0, h};
		while (hcns(aa_foreach)(tmp2)) {
			struct hcns(aa) *tmp = *tmp2;
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
