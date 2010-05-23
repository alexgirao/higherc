#include <stdlib.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/s.h"
#include "higherc/bytewise.h"

/* alloc/free
 */

hcns(bool) hcns(s_alloc)(struct hcns(s) *x, int n)
{
	if (x->s) {
		if (n > x->a) {
			int i = n + (n >> 3) + 30;
			void *p = hcns(alloc_re)(x->s, x->len, i);
			if (p) {
				x->a = i;
				x->s = p;
				return 1;
			}
			HC_FATAL("memory allocation failed"); /* it's better be safer than sorry */
			return 0;
		}
		return 1;
	}
	x->s = hcns(alloc)(n);
	if (x->s) {
		x->a = n;
		x->len = 0;
		return 1;
	}
	HC_FATAL("memory allocation failed"); /* it's better be safer than sorry */
	return 0;
}

hcns(bool) hcns(s_free)(struct hcns(s) *x)
{
	if (x->s) {
		void *p = x->s;
		x->s = NULL;
		x->len = 0;
		x->a = 0;
		hcns(alloc_free)(p);
	}
	return 1;
}

/* copy
 */

hcns(bool) hcns(s_copyn)(struct hcns(s) *sa, const char *s, int n)
{
	if (!hcns(s_alloc)(sa, n + 1)) {
		return 0;
	}
	hcns(bcopyl)(sa->s, n, s);
	sa->len = n;
	sa->s[n] = 'Z';		/* ``offensive programming'' */
	return 1;
}

hcns(bool) hcns(s_copy)(struct hcns(s) *to, const struct hcns(s) *from)
{
	return hcns(s_copyn)(to, from->s, from->len);
}

hcns(bool) hcns(s_copyz)(struct hcns(s) *sa, const char *s)
{
	return hcns(s_copyn)(sa, s, hcns(slen)(s));
}

/* cat
 */

hcns(bool) hcns(s_catn)(struct hcns(s) *sa, const char *s, int n)
{
	if (!sa->s) {
		return hcns(s_copyn)(sa, s, n);
	}
	if (!hcns(s_alloc)(sa, sa->len + n + 1)) {
		return 0;
	}
	hcns(bcopyl)(sa->s + sa->len, n, s);
	sa->len += n;
	sa->s[sa->len] = 'Z';	/* ``offensive programming'' */
	return 1;
}

hcns(bool) hcns(s_cat)(struct hcns(s) *to, const struct hcns(s) *from)
{
	return hcns(s_catn)(to, from->s, from->len);
}

hcns(bool) hcns(s_catz)(struct hcns(s) *sa, const char *s)
{
	return hcns(s_catn)(sa, s, hcns(slen)(s));
}

/* cat numeric values
 */

void hcns(s_cat_u4_dec)(HC_ST_S *s, hcns(u4) v)
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
	hcns(brev)(s->s + s_len0, s->len - s_len0);
}

void hcns(s_cat_i4_dec)(HC_ST_S *s, hcns(i4) v)
{
	if (v < 0) {
		hcns(s_catn)(s, "-", 1);
		v = -v;
	}
	hcns(s_cat_u4_dec)(s, v);
}

/* diff
 */

int hcns(s_diffn)(struct hcns(s) *a, char *b, int bl)
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

int hcns(s_diffz)(struct hcns(s) *a, char *b)
{
	hcns(s_catn)(a, "\0", 1);
	return hcns(sdiffn)(a->s, b, a->len--);
}

int hcns(s_diff)(HC_ST_S *a, HC_ST_S *b)
{
	return hcns(sdiffn)(a->s, b->s, b->len);
}

/* case change
 */

void hcns(s_upper)(struct hcns(s) *s)
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

void hcns(s_lower)(struct hcns(s) *s)
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
