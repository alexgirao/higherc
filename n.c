#include <stdlib.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/bytewise.h"
#include "higherc/n.h"

/* alloc/free
 */

hcns(bool) hcns(n_alloc)(struct hcns(n) *x, int n)
{
	if (x->d) {
		if (n > x->a) {
			int i = n + 2;
			void *p = hcns(alloc_re)(x->d, x->len * sizeof(hcns(h)), i * sizeof(hcns(h)));
			if (p) {
				x->a = i;
				x->d = p;
				return 1;
			}
			return 0;
		}
		return 1;
	}
	x->d = hcns(alloc)(n * sizeof(hcns(h)));
	if (x->d) {
		x->a = n;
		x->len = 0;
		return 1;
	}
	return 0;
}

hcns(bool) hcns(n_free)(struct hcns(n) *x)
{
	if (x->d) {
		void *p = x->d;
		x->d = NULL;
		x->len = 0;
		x->a = 0;
		hcns(alloc_free)(p);
	}
	return 1;
}

/* copy
 */

hcns(bool) hcns(n_copyn)(struct hcns(n) *x, const hcns(h) *d, int n)
{
	if (!hcns(n_alloc)(x, (n + 1) * sizeof(hcns(h))))
		return 0;
	hcns(bcopyl)(x->d, n * sizeof(hcns(h)), d);
	x->len = n;
	x->d[x->len] = HC_HALF_OFFENSE;   /* ``offensive programming'' */ 
	return 1;
}

hcns(bool) hcns(n_copy)(struct hcns(n) *to, const struct hcns(n) *from)
{
	return hcns(n_copyn)(to, from->d, from->len);
}
