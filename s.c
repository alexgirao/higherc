#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/s.h"
#include "higherc/bytewise.h"

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
	hcns(s_alloc)(s, s->len + 10);  /* 4-bytes can't hold more
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
