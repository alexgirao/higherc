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
#include "higherc/tagid.h"

/* see ~/projects/tagdb/README2
 */

struct tagid {
	HC_ST_S A[1];
	int B;
	int C;
	unsigned int D;
	unsigned char E[20];  /* sha1 bytes */
};

#define NULL_TAGID {{HC_NULL_S}, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}

static void tagid_set(struct tagid *tagid, struct hcns(tag) *tag)
{
	struct hcns(tag) **taglist = hcns(tag_as_array)(tag);
	int i, len = hcns(tag_usort)(taglist, hcns(tag_len)(tag));
	HC_DEF_SHA1(E0);

	for (i=0; i<len; i++) {
		hcns(s_cat)(tagid->A, taglist[i]->value);
		if (i < len-1) {
			hcns(s_catn)(tagid->A, "-", 1);
		}
	}

	tagid->B = len;
	tagid->C = tagid->A->len;
	tagid->D = hcns(crc32)(0, tagid->A->s, tagid->A->len);
	
	hcns(sha1_init)(E0);
	hcns(sha1_update)(E0, tagid->A->s, tagid->A->len);
	hcns(sha1_final)(E0, tagid->E);

	HC_FREE(taglist);
}

static void tagid_free(struct tagid *tagid)
{
	hcns(s_free)(tagid->A);
}

int main(int argc, char **argv)
{
	int i;
	struct hcns(tag) *h = NULL;
	struct tagid tagid[1] = {NULL_TAGID};

	for (i=1; i<argc; i++) {
		h = hcns(tag_new)(h, argv[i]);
		if (h == NULL) {
			HC_FATAL("invalid tag at arg %i", i);
		}
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	tagid_set(tagid, h);

	HC_SAFE_CSTR(tagid->A);
	printf("tagid.A: %s\n", tagid->A->s);
	printf("tagid.B: %i\n", tagid->B);
	printf("tagid.C: %i\n", tagid->C);
	printf("tagid.D: %.8x\n", tagid->D);

	{
		HC_DEF_S(E);
		hcns(n_be1_as_base36)(E, tagid->E, sizeof(tagid->E));
		HC_SAFE_CSTR(E);
		printf("tagid.E: %s\n", E->s);
		hcns(s_free)(E);
	}

        /* cleanup
	 */

	hcns(tag_free)(h);
	tagid_free(tagid);

	return 0;
}
