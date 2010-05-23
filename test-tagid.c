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
#include "higherc/n.h"
#include "higherc/sha1.h"
#include "higherc/crc32.h"

HC_DECL_PRIVATE_I(i,
		  HC_ST_S tag;
)

#define CMP hcns(s_bdiff)(&a->tag, b->tag.s, b->tag.len)
HC_DECL_PRIVATE_I_USORT(i, a, b, CMP)

/* see ~/projects/tagdb/README2
 */

struct tagid {
	struct hcns(s) A;
	int B;
	int C;
	unsigned int D;
	unsigned char E[20];  /* sha1 bytes */
};

#define NULL_TAGID {HC_NULL_S, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}

static void insertion_sort(struct i **tagset, int len)
{
	int i;
	struct i *input;
	for(i=1; i<len; i++) {
		int j = i - 1;
		input = tagset[i];
		assert(input != NULL);
		assert(input->tag.s != NULL);
		while (j>=0 && hcns(s_bdiff)(&tagset[j]->tag, input->tag.s, input->tag.len) > 0) {
			assert(tagset[j] != NULL);
			assert(tagset[j]->tag.s != NULL);
			/* shift down elements greater than input
			 */
			tagset[j+1] = tagset[j];
			j--;
		}

		if (j+1 != i) {
			/* don't swap if input is already in order
			 */
			tagset[j+1] = input;
		}
	}
}

/* returns new size
 */
static int no_duplicates(struct i **tagset, int len)
{
	int i=1;
	while (i < len) {
		if (hcns(s_bdiff)(&tagset[i - 1]->tag, tagset[i]->tag.s, tagset[i]->tag.len) == 0) {
			/* duplicate found, decrease len
			 */
			int j = i + 1;
			while (j < len) {
				/* shift up
				 */
				tagset[j - 1] = tagset[j];
				j++;
			}
			len--;
		} else {
			/* increase pos
			 */
			i++;
		}
	}
	return len;
}

static hcns(bool) is_valid_tag(HC_ST_S *tag)
{
	int i;
	for (i=0; i<tag->len; i++) {
		if (!HC_ISALNUM(tag->s[i])) {
			return HC_FALSE;
		}
	}
	return HC_TRUE;
}

int main(int argc, char **argv)
{
	int i;
	struct i *h = NULL, *t;
	struct i_iter iter[1];
	struct tagid tagid = NULL_TAGID;

	/* fill list
	 */

	for (i=1; i<argc; i++) {
		h = i_new(h);
		hcns(s_copyz)(&h->tag, argv[i]);
		hcns(s_lower)(&h->tag); /* tags are lower case only */
		if (!is_valid_tag(&h->tag)) {
			HC_FATAL("invalid tag at arg %i", i);
		}
		HC_SAFE_CSTR(&h->tag);
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	/* reverse
	 */

	struct i **tagset = i_as_array(h);
	int len;
	i_usort(tagset, h->pos + 1, &len);

	//insertion_sort(tagset, h->pos + 1);
	//int len = no_duplicates(tagset, h->pos + 1);

	

	for (i=0; i<len; i++) {
		struct i *tmp = tagset[i];

		hcns(s_cat)(&tagid.A, &tmp->tag);
		if (i < len-1) {
			hcns(s_catn)(&tagid.A, "-", 1);
		}
	}
	HC_SAFE_CSTR(&tagid.A);

	tagid.B = len;
	tagid.C = tagid.A.len;
	tagid.D = hcns(crc32)(0, tagid.A.s, tagid.A.len);
	
	{
		HC_DEF_SHA1(E0);
		hcns(sha1_init)(E0);
		hcns(sha1_update)(E0, tagid.A.s, tagid.A.len);
		hcns(sha1_final)(E0, tagid.E);
	}

	/*
	 */

	printf("tagid.A: %s\n", tagid.A.s);
	printf("tagid.B: %i\n", tagid.B);
	printf("tagid.C: %i\n", tagid.C);
	printf("tagid.D: %.8x\n", tagid.D);

	{
		HC_DEF_S(E);
		hcns(n_be1_as_base36)(E, tagid.E, sizeof(tagid.E));
		printf("tagid.E: %s\n", E->s);
		hcns(s_free)(E);
	}

        /* cleanup
	 */

	i_backward(h, iter);
	while ((t = i_next(iter))) {
		hcns(s_free)(&t->tag);
	}
	i_end(iter);

	i_free(h);
	HC_FREE(tagset);
	hcns(s_free)(&tagid.A);

	return 0;
}
