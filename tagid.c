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

HC_DECL_PUBLIC_I(tag);

#define CMP_EXPR s_diff(a->value, b->value)

HC_DECL_PUBLIC_I_USORT(tag, a, b, CMP_EXPR);

struct tag *tag_new(struct tag *x, HC_ST_S *s)
{
	HC_ST_TAG *r = tag_new0(x);
	if (!tag_set(r, s)) {
		s_free(r->value);
		return NULL;
	}
	return r;
}

struct tag *tag_newz(struct tag *x, char *z)
{
	HC_ST_TAG *r = tag_new0(x);
	if (!tag_setz(r, z)) {
		s_free(r->value);
		return NULL;
	}
	return r;
}

void tag_free(struct tag *x)
{
	struct tag_iter iter[1];
	HC_ST_TAG *t;

	tag_backward(iter, x);
	while ((t = tag_next(iter))) {
		s_free(t->value);
	}
	tag_end(iter);

	tag_free0(x);
}

bool tag_is_valid(struct s *s)
{
	int i;
	for (i=0; i<s->len; i++) {
		if (!HC_ISALNUM(s->s[i])) {
			return HC_FALSE;
		}
	}
	return HC_TRUE;
}

bool tag_set(struct tag *x, HC_ST_S *s)
{
	s_copy(x->value, s);
	s_lower(x->value); /* tags are lower case only */
	return tag_is_valid(x->value);
}

bool tag_setz(struct tag *x, char *z)
{
	s_copyz(x->value, z);
	s_lower(x->value); /* tags are lower case only */
	return tag_is_valid(x->value);
}

/* tagid
 */

void tagid_set_tags0(HC_ST_TAGID *tagid, HC_ST_S *A, HC_ST_TAG *tag)
{
	HC_ST_TAG **taglist;
	int i, len;
	HC_DEF_SHA1(E0);

	assert(tag);

	A->len = 0;

	taglist = tag_as_array(tag);
	len = tag_usort(taglist, tag_len(tag));

	s_copy(A, taglist[0]->value);
	for (i=1; i<len; i++) {
		s_catc(A, '-');
		s_cat(A, taglist[i]->value);
	}

	tagid->B = len;
	tagid->C = A->len;
	tagid->D = crc32(0, A->s, A->len);

	sha1_init(E0);
	sha1_update(E0, A->s, A->len);
	sha1_final(E0, tagid->E);

	HC_FREE(taglist);
}

void tagid_set_tags(HC_ST_TAGID *tagid, HC_ST_TAG *tag)
{
	HC_DEF_S(A);
	tagid_set_tags0(tagid, A, tag);
	s_free(A);
}

void tagid_cat_id(HC_ST_TAGID *tagid, HC_ST_S *tid)
{
	int r;
	s_copyn(tid, "tid", 3);

	r = s_cat_u4_hex(tid, tagid->B & 0xff);
	s_shiftr2(tid, -r, 2 - r, '0');

	r = s_cat_u4_hex(tid, tagid->C & 0xff);
	s_shiftr2(tid, -r, 2 - r, '0');

	r = s_cat_u4_hex(tid, tagid->D);
	s_shiftr2(tid, -r, 8 - r, '0');

	r = n_be1_as_base36(tid, tagid->E, sizeof(tagid->E));
	s_shiftr2(tid, -r, 31 - r, '0');
}

void tagid_free(HC_ST_TAGID *tagid)
{
	/* nothing to do, result api evolution
	 */
}

/* serialization
 */

void tagid_put(HC_ST_TAGID *tagid, void *out)
{
	unsigned char *x = out;
	*x++ = tagid->B & 0xff;  /* 0 + 1: 1 */
	*x++ = tagid->C & 0xff;  /* 1 + 1: 2 */
	HC_PUT_BE4(x, tagid->D); /* 2 + 4: 6 */
	b_copyl(x + 4, sizeof(tagid->E), tagid->E); /* 6 + 20: 26 */
}

void tagid_get(HC_ST_TAGID *tagid, void *in)
{
	unsigned char *x = in;
	tagid->B = *x++;
	tagid->C = *x++;
	tagid->D = HC_GET_BE4(x);
	b_copyl(tagid->E, sizeof(tagid->E), x + 4);
}
