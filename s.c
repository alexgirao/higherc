#include <stdlib.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/stralloc.h"
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
	if (!hcns(s_alloc)(sa, n + 1))
		return 0;
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
	if (!sa->s)
		return hcns(s_copyn)(sa, s, n);
	if (!hcns(s_alloc)(sa, sa->len + n + 1))
		return 0;
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

/* diff
 */

int hcns(s_bdiff)(struct hcns(s) *a, struct hcns(s) *b)
{
	int x = a->len - b->len;
	int y = 0;

	if (x > 0) {
		x = 1;
		y = hcns(bdiff)(a->s, b->len, b->s);
	} else if (x < 0) {
		x = -1;
		y = hcns(bdiff)(a->s, a->len, b->s);
	} else {
		y = hcns(bdiff)(a->s, a->len, b->s);
	}
	return y ? y : x;
}

int hcns(s_sdiff)(struct hcns(s) *a, char *b)
{
	hcns(s_catn)(a, "\0", 1);
	return hcns(sdiffn)(a->s, b, a->len--);
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