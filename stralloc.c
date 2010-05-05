#include <stdlib.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/stralloc.h"

/* alloc/free
 */

hcns(bool) hcns(stralloc_alloc)(struct hcns(stralloc) *x, int n)
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

hcns(bool) hcns(stralloc_free)(struct hcns(stralloc) *x)
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

hcns(bool) hcns(stralloc_copyn)(struct hcns(stralloc) *sa, const char *s, int n)
{
	if (!hcns(stralloc_alloc)(sa, n + 1))
		return 0;
	hcns(byte_copyl)(sa->s, n, s);
	sa->len = n;
	sa->s[n] = 'Z';		/* ``offensive programming'' */
	return 1;
}

hcns(bool) hcns(stralloc_copy)(struct hcns(stralloc) *to, const struct hcns(stralloc) *from)
{
	return hcns(stralloc_copyn)(to, from->s, from->len);
}

hcns(bool) hcns(stralloc_copyz)(struct hcns(stralloc) *sa, const char *s)
{
	return hcns(stralloc_copyn)(sa, s, hcns(str_len)(s));
}

/* cat
 */

hcns(bool) hcns(stralloc_catn)(struct hcns(stralloc) *sa, const char *s, int n)
{
	if (!sa->s)
		return hcns(stralloc_copyn)(sa, s, n);
	if (!hcns(stralloc_alloc)(sa, sa->len + n + 1))
		return 0;
	hcns(byte_copyl)(sa->s + sa->len, n, s);
	sa->len += n;
	sa->s[sa->len] = 'Z';	/* ``offensive programming'' */
	return 1;
}

hcns(bool) hcns(stralloc_cat)(struct hcns(stralloc) *to, const struct hcns(stralloc) *from)
{
	return hcns(stralloc_catn)(to, from->s, from->len);
}

hcns(bool) hcns(stralloc_catz)(struct hcns(stralloc) *sa, const char *s)
{
	return hcns(stralloc_catn)(sa, s, hcns(str_len)(s));
}
