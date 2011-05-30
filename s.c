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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/s.h"
#include "higherc/bytewise.h"
#include "higherc/sha1.h"

/* alloc/free
 */

void hcns(s_alloc)(HC_ST_S *x, int n)
{
	if (x->s) {
		if (n > x->a) {
			int i = n + (n >> 3) + 30;
			void *p = hcns(alloc_re)(x->s, x->len, i);
			if (p) {
				x->a = i;
				x->s = p;
				return;
			}
			HC_FATAL("memory allocation failed"); /* it's better be safer than sorry */
		}
		return;
	}
	HC_ALLOC(x->s, n);
	x->a = n;
	x->len = 0;
}

void hcns(s_free)(HC_ST_S *x)
{
	if (x->s) {
		void *p = x->s;
		x->s = NULL;
		x->len = 0;
		x->a = 0;
		HC_FREE(p);
	}
}

/* copy
 */

void hcns(s_copyn)(HC_ST_S *sa, const char *s, int n)
{
	hcns(s_alloc)(sa, n + 1);
	hcns(bcopyl)(sa->s, n, s);
	sa->len = n;
	sa->s[n] = 'Z';		/* ``offensive programming'' */
}

void hcns(s_copy)(HC_ST_S *to, const HC_ST_S *from)
{
	hcns(s_copyn)(to, from->s, from->len);
}

void hcns(s_copyz)(HC_ST_S *sa, const char *s)
{
	hcns(s_copyn)(sa, s, hcns(slen)(s));
}

void hcns(s_copyc)(HC_ST_S *sa, int c)
{
	if (sa->len == 0) {
		hcns(s_alloc)(sa, 1);
	}
	sa->s[0] = c;
	sa->len = 1;
}

/* cat
 */

void hcns(s_catn)(HC_ST_S *sa, const char *s, int n)
{
	if (!sa->s) {
		hcns(s_copyn)(sa, s, n);
		return;
	}
	hcns(s_alloc)(sa, sa->len + n + 1);
	hcns(bcopyl)(sa->s + sa->len, n, s);
	sa->len += n;
	sa->s[sa->len] = 'Z';	/* ``offensive programming'' */
}

void hcns(s_cat)(HC_ST_S *to, const HC_ST_S *from)
{
	hcns(s_catn)(to, from->s, from->len);
}

void hcns(s_catz)(HC_ST_S *sa, const char *s)
{
	hcns(s_catn)(sa, s, hcns(slen)(s));
}

void hcns(s_catc)(HC_ST_S *sa, int c)
{
	if (sa->len == sa->a) {
		hcns(s_alloc)(sa, sa->len + 1);
	}
	sa->s[sa->len++] = c;
}

/* format
 */

void hcns(s_vformat)(HC_ST_S *sa, hcns(bool) cat, const char *fmt, va_list va)
{
	char buf0[0x1fff], *buf;
	int buf_len;
	int n;

	buf = buf0;
	buf_len = sizeof(buf);

	n = vsnprintf(buf, buf_len, fmt, va);
	if (n >= buf_len) {
		/* truncated, try with more space
		 */
		buf_len = 0x7fff;
		HC_ALLOC(buf, buf_len);
		n = vsnprintf(buf, buf_len, fmt, va);
		if (n >= buf_len) {
			HC_FREE(buf);
			buf_len = 0x1ffff;
			HC_ALLOC(buf, buf_len);
			n = vsnprintf(buf, buf_len, fmt, va);
			if (n >= buf_len) {
				HC_FREE(buf);
				buf_len = 0x7fffff; /* 8388607 bytes! */
				HC_ALLOC(buf, buf_len);
				n = vsnprintf(buf, buf_len, fmt, va);
				if (n >= buf_len) {
					/* give up
					 */
					HC_FREE(buf);
					fprintf(stderr, "error: str_copyf(): too large input (> %i)\n", 0x7fffff);
					exit(1);
				}
			}
		}
	}

	assert(buf != NULL);

	if (cat) {
		hcns(s_catn)(sa, buf, n);
	} else {
		hcns(s_copyn)(sa, buf, n);
	}

	HC_FREE(buf);
}

void hcns(s_copyf)(HC_ST_S *sa, const char *fmt, ...)
{
	va_list	va;
	va_start(va, fmt);
	hcns(s_vformat)(sa, 0, fmt, va);
	va_end(va);
}

void hcns(s_catf)(HC_ST_S *sa, const char *fmt, ...)
{
	va_list	va;
	va_start(va, fmt);
	hcns(s_vformat)(sa, 1, fmt, va);
	va_end(va);
}

/* cat numeric values
 */

int hcns(s_cat_u4_hex)(HC_ST_S *s, hcns(u4) v)
{
	int s_len0 = s->len;
	hcns(s_alloc)(s, s->len + 8);  /* 0x12345678 */
	while (v) {
		s->s[s->len++] = HC_HEX_DIGIT(v & 0xf);
		v = v >> 4;
	}
	if (s->len == s_len0) {
		hcns(s_catn)(s, "0", 1);
	} else {
		hcns(brev)(s->s + s_len0, s->len - s_len0);
	}
	return s->len - s_len0;
}

int hcns(s_cat_i4_hex)(HC_ST_S *s, hcns(i4) v)
{
	int i = 0;
	if (v < 0) {
		i++;
		hcns(s_catn)(s, "-", 1);
		v = -v;
	}
	return hcns(s_cat_u4_hex)(s, v) + i;
}

int hcns(s_cat_u4_dec)(HC_ST_S *s, hcns(u4) v)
{
	int s_len0 = s->len;
	hcns(s_alloc)(s, s->len + 10);	/* 4-bytes can't hold more
					 * than 10 decimal digits (max
					 * decimal value is
					 * 4294967296) */
	while (v) {
		s->s[s->len++] = HC_DEC_DIGIT(v % 10);
		v = v / 10;
	}
	if (s->len == s_len0) {
		hcns(s_catn)(s, "0", 1);
	} else {
		hcns(brev)(s->s + s_len0, s->len - s_len0);
	}
	return s->len - s_len0;
}

int hcns(s_cat_i4_dec)(HC_ST_S *s, hcns(i4) v)
{
	int i = 0;
	if (v < 0) {
		i++;
		hcns(s_catn)(s, "-", 1);
		v = -v;
	}
	return hcns(s_cat_u4_dec)(s, v) + i;
}

int hcns(s_cat_u4_base36)(HC_ST_S *s, hcns(u4) v)
{
	int s_len0 = s->len;
	hcns(s_alloc)(s, s->len + 7);  /* 0xffffffff = 1z141z3 */
	while (v) {
		s->s[s->len++] = HC_BASE36_DIGIT(v % 36);
		v = v / 36;
	}
	if (s->len == s_len0) {
		hcns(s_catn)(s, "0", 1);
	} else {
		hcns(brev)(s->s + s_len0, s->len - s_len0);
	}
	return s->len - s_len0;
}

int hcns(s_cat_i4_base36)(HC_ST_S *s, hcns(i4) v)
{
	int i = 0;
	if (v < 0) {
		i++;
		hcns(s_catn)(s, "-", 1);
		v = -v;
	}
	return hcns(s_cat_u4_base36)(s, v) + i;
}

/* diff
 */

int hcns(s_diffn)(HC_ST_S *a, char *b, int bl)
{
	int x = a->len - bl;
	int y = 0;

	if (x > 0) {
		x = 1;
		y = hcns(bdiff)(a->s, bl, b);
	} else if (x < 0) {
		x = -1;
		y = hcns(bdiff)(a->s, a->len, b);
	} else {
		y = hcns(bdiff)(a->s, a->len, b);
	}
	return y ? y : x;
}

int hcns(s_diffz)(HC_ST_S *a, char *b)
{
	hcns(s_catn)(a, "\0", 1);
	return hcns(sdiffn)(a->s, b, a->len--);
}

int hcns(s_diff)(HC_ST_S *a, HC_ST_S *b)
{
	return hcns(s_diffn)(a, b->s, b->len);
}

/* case change
 */

void hcns(s_upper)(HC_ST_S *s)
{
	int i;
	char c;
	for (i=0; i<s->len; i++, c++) {
		c = s->s[i];
		if (HC_ISALPHA(c)) {
			s->s[i] = HC_TOUPPER(c);
		}
	}
}

void hcns(s_lower)(HC_ST_S *s)
{
	int i;
	char c;
	for (i=0; i<s->len; i++, c++) {
		c = s->s[i];
		if (HC_ISALPHA(c)) {
			s->s[i] = HC_TOLOWER(c);
		}
	}
}

/* shift
 */

void hcns(s_shiftr)(HC_ST_S *s, int start, int end, int n, int pad)
{
	int i, window_size;
	char *ss;

	if (start < 0) {
		start = s->len + start;
	}

	if (end < 0) {
		end = s->len + end;
	}

	assert(n >= 0);
	assert(start >= 0);
	assert(start <= s->len);

	window_size = end - start;

	assert(end >= 0);
	assert(start <= end);
	assert(n <= window_size);

	if (end > s->len) {
		/* end is exclusive, so its len compatible
		 */
		hcns(s_alloc)(s, end);
		s->len = end;
	}
	ss = s->s + start;
	hcns(bcopyr)(ss + n, window_size - n, ss);

	for (i=0; i<n; i++) {
		ss[i] = pad;
	}
}

void hcns(s_shiftl)(HC_ST_S *s, int start, int end, int n, int pad)
{
	int i, window_size;
	char *ss;

	if (start < 0) {
		start = s->len + start;
	}

	if (end < 0) {
		end = s->len + end;
	}

	assert(n >= 0);
	assert(start >= 0);
	assert(start <= s->len);

	window_size = end - start;

	assert(end >= 0);
	assert(start <= end);
	assert(n <= window_size);

	assert(end <= s->len);

	ss = s->s + start;
	hcns(bcopyl)(ss, window_size - n, ss + n);

	ss += window_size - n;
	for (i=0; i<n; i++) {
		ss[i] = pad;
	}
}

void hcns(s_shiftr2)(HC_ST_S *s, int start, int n, int pad)
{
	hcns(s_shiftr)(s, start, s->len + n, n, pad);
}

void hcns(s_shiftl2)(HC_ST_S *s, int start, int n, int pad)
{
	hcns(s_shiftl)(s, start, s->len, n, pad);
}

/* repr
 */

void hcns(s_reprn)(HC_ST_S *s_repr, void *s, int n)
{
	unsigned char *x = s;
	int c, flags;
	while (n--) {
		c = *x++;
		flags = HC_CTYPE_FLAGS(c);
		if (flags & (HC_ALNUM | HC_PUNCT | HC_BLANK)) {
			hcns(s_catc)(s_repr, c);
		} else {
			hcns(s_alloc)(s_repr, s_repr->len + 4); /* \xNN */
			s_repr->s[s_repr->len++] = '\\';
			s_repr->s[s_repr->len++] = 'x';
			s_repr->s[s_repr->len++] = HC_HEX_DIGIT((c >> 4) & 0xf);
			s_repr->s[s_repr->len++] = HC_HEX_DIGIT(c & 0xf);
		}
	}
}

void hcns(s_repr)(HC_ST_S *s_repr, HC_ST_S *s)
{
	hcns(s_reprn)(s_repr, s->s, s->len);
}

void hcns(s_reprz)(HC_ST_S *s_repr, char *s)
{
	hcns(s_reprn)(s_repr, s, hcns(slen)(s));
}

/* serialization
 */

int hcns(s_putlen)(HC_ST_S *x)
{
	return hcns(enc_u4_be)(NULL, x->len) + x->len;
}

int hcns(s_put)(HC_ST_S *x, void *out)
{
	int r;
	r = hcns(enc_u4_be)(out, x->len);
	hcns(bcopyl)(HC_OFFSET(out, r), x->len, x->s);
	return r + x->len;
}

int hcns(s_get)(HC_ST_S *x, void *in)
{
	int r;
	int len = hcns(dec_u4_be)(in, &r);
	hcns(s_alloc)(x, len);
	x->len = len;
	hcns(bcopyl)(x->s, len, HC_OFFSET(in, r));
	return r + len;
}

/* sha1
 */

int hcns(s_cat_sha1hex)(HC_ST_S *x, void *s, int n)
{
	HC_DEF_SHA1(E0);
	unsigned char E[20];  /* sha1 bytes */

	hcns(sha1_init)(E0);
	hcns(sha1_update)(E0, s, n);
	hcns(sha1_final)(E0, E);

	hcns(s_alloc)(x, x->len + 40 + 1);
	HC_PUT_HEX(x->s + x->len, 20, E);
	x->len += 40;

	x->s[40] = 'Z';		/* ``offensive programming'' */

	return 40;
}
