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

#define CMP_EXPR hcns(s_bdiff)(a->value, b->value->s, b->value->len)

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
	if (!hcns(s_copyz)(x->value, z)) {
		HC_FATAL("hcns(s_copyz)(%p, %p)", x, z);
	}
	hcns(s_lower)(x->value); /* tags are lower case only */
	return hcns(tag_is_valid)(x->value);
}
