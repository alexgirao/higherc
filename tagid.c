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
#include "higherc/n.h"
#include "higherc/sha1.h"
#include "higherc/crc32.h"
#include "higherc/tagid.h"

HC_DECL_PUBLIC_I(hcns(tag));

#define CMP_EXPR hcns(s_diff)(a->value, b->value)

HC_DECL_PUBLIC_I_USORT(hcns(tag), a, b, CMP_EXPR);

struct hcns(tag) *hcns(tag_new)(struct hcns(tag) *x, HC_ST_S *s)
{
	HC_ST_TAG *r = hcns(tag_new0)(x);
	if (!hcns(tag_set)(r, s)) {
		hcns(s_free)(r->value);
		return NULL;
	}
	return r;
}

struct hcns(tag) *hcns(tag_newz)(struct hcns(tag) *x, char *z)
{
	HC_ST_TAG *r = hcns(tag_new0)(x);
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

	hcns(tag_backward)(iter, x);
	while ((t = hcns(tag_next)(iter))) {
		hcns(s_free)(t->value);
	}
	hcns(tag_end)(iter);

	hcns(tag_free0)(x);
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

hcns(bool) hcns(tag_set)(struct hcns(tag) *x, HC_ST_S *s)
{
	hcns(s_copy)(x->value, s);
	hcns(s_lower)(x->value); /* tags are lower case only */
	return hcns(tag_is_valid)(x->value);
}

hcns(bool) hcns(tag_setz)(struct hcns(tag) *x, char *z)
{
	hcns(s_copyz)(x->value, z);
	hcns(s_lower)(x->value); /* tags are lower case only */
	return hcns(tag_is_valid)(x->value);
}

/* tagid
 */

void hcns(tagid_set_tags0)(HC_ST_TAGID *tagid, HC_ST_S *A, HC_ST_TAG *tag)
{
	HC_ST_TAG **taglist;
	int i, len;
	HC_DEF_SHA1(E0);

	assert(tag);

	A->len = 0;

	taglist = hcns(tag_as_array)(tag);
	len = hcns(tag_usort)(taglist, hcns(tag_len)(tag));

	hcns(s_copy)(A, taglist[0]->value);
	for (i=1; i<len; i++) {
		hcns(s_catc)(A, '-');
		hcns(s_cat)(A, taglist[i]->value);
	}

	tagid->B = len;
	tagid->C = A->len;
	tagid->D = hcns(crc32)(0, A->s, A->len);

	hcns(sha1_init)(E0);
	hcns(sha1_update)(E0, A->s, A->len);
	hcns(sha1_final)(E0, tagid->E);

	HC_FREE(taglist);
}

void hcns(tagid_set_tags)(HC_ST_TAGID *tagid, HC_ST_TAG *tag)
{
	HC_DEF_S(A);
	hcns(tagid_set_tags0)(tagid, A, tag);
	hcns(s_free)(A);
}

void hcns(tagid_cat_id)(HC_ST_TAGID *tagid, HC_ST_S *tid)
{
	int r;
	hcns(s_copyn)(tid, "tid", 3);

	r = hcns(s_cat_u4_hex)(tid, tagid->B & 0xff);
	hcns(s_shiftr2)(tid, -r, 2 - r, '0');

	r = hcns(s_cat_u4_hex)(tid, tagid->C & 0xff);
	hcns(s_shiftr2)(tid, -r, 2 - r, '0');

	r = hcns(s_cat_u4_hex)(tid, tagid->D);
	hcns(s_shiftr2)(tid, -r, 8 - r, '0');

	r = hcns(n_be1_as_base36)(tid, tagid->E, sizeof(tagid->E));
	hcns(s_shiftr2)(tid, -r, 31 - r, '0');
}

void hcns(tagid_free)(HC_ST_TAGID *tagid)
{
	/* nothing to do, result api evolution
	 */
}

/* serialization
 */

void hcns(tagid_put)(HC_ST_TAGID *tagid, void *out)
{
	unsigned char *x = out;
	*x++ = tagid->B & 0xff;  /* 0 + 1: 1 */
	*x++ = tagid->C & 0xff;  /* 1 + 1: 2 */
	HC_PUT_BE4(x, tagid->D); /* 2 + 4: 6 */
	hcns(bcopyl)(x + 4, sizeof(tagid->E), tagid->E); /* 6 + 20: 26 */
}

void hcns(tagid_get)(HC_ST_TAGID *tagid, void *in)
{
	unsigned char *x = in;
	tagid->B = *x++;
	tagid->C = *x++;
	tagid->D = HC_GET_BE4(x);
	hcns(bcopyl)(tagid->E, sizeof(tagid->E), x + 4);
}
