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

HC_DECL_PUBLIC_I(hcns(tag));

#define CMP_EXPR hcns(s_diff)(a->value, b->value)

HC_DECL_PUBLIC_I_USORT(hcns(tag), a, b, CMP_EXPR);

struct hcns(tag) *hcns(tag_new)(struct hcns(tag) *x, char *z)
{
	HC_ST_TAG *r = hcns(tag__new)(x);
	if (!hcns(tag_setz)(r, z)) {
		hcns(s_free)(r->value);
		return NULL;
	}
	return r;
}

void hcns(tag_free)(struct hcns(tag) *x)
{
	struct hcns(tag_iter) iter[1];
	HC_ST_TAG *t;

	hcns(tag_backward)(x, iter);
	while ((t = hcns(tag_next)(iter))) {
		hcns(s_free)(t->value);
	}
	hcns(tag_end)(iter);

	hcns(tag__free)(x);
}

hcns(bool) hcns(tag_is_valid)(struct hcns(s) *s)
{
	int i;
	for (i=0; i<s->len; i++) {
		if (!HC_ISALNUM(s->s[i])) {
			return HC_FALSE;
		}
	}
	return HC_TRUE;
}

hcns(bool) hcns(tag_setz)(struct hcns(tag) *x, char *z)
{
	hcns(s_copyz)(x->value, z);
	hcns(s_lower)(x->value); /* tags are lower case only */
	return hcns(tag_is_valid)(x->value);
}

/* tagid
 */

void hcns(tagid_set)(HC_ST_TAGID *tagid, HC_ST_TAG *tag)
{
	HC_ST_TAG **taglist;
	int i, len;
	HC_DEF_SHA1(E0);

	assert(tag);

	taglist = hcns(tag_as_array)(tag);
	len = hcns(tag_usort)(taglist, hcns(tag_len)(tag));

	hcns(s_cat)(tagid->A, taglist[0]->value);
	for (i=1; i<len; i++) {
		hcns(s_catn)(tagid->A, "-", 1);
		hcns(s_cat)(tagid->A, taglist[i]->value);
	}

	tagid->B = len;
	tagid->C = tagid->A->len;
	tagid->D = hcns(crc32)(0, tagid->A->s, tagid->A->len);

	hcns(sha1_init)(E0);
	hcns(sha1_update)(E0, tagid->A->s, tagid->A->len);
	hcns(sha1_final)(E0, tagid->E);

	HC_FREE(taglist);
}

void hcns(tagid_free)(HC_ST_TAGID *tagid)
{
	hcns(s_free)(tagid->A);
}
