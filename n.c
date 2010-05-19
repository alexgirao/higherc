#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/bytewise.h"
#include "higherc/fatal.h"
#include "higherc/s.h"
#include "higherc/n.h"

/* alloc/free
 */

hcns(bool) hcns(n_alloc)(struct hcns(n) *x, int n)
{
	if (x->d) {
		if (n > x->a) {
			//int i = n + 2;
			int i = n;
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

/*
 */

#if (HC_HALF_BYTES == 2)

void hcns(n_set_u4)(struct hcns(n) *n, hcns(u4) v)
{
	if (!hcns(n_alloc)(n, 2)) {
		HC_FATAL("n_alloc(%p, %i) failed", n, 2);
	}
	n->d[0] = HC_LOW(v);
	n->d[1] = HC_HIGH(v);
	n->len = 2;
}

void hcns(n_load_be1)(struct hcns(n) *r, void *x, int len)
{
	int i, ndigits = len / HC_HALF_BYTES;
	hcns(u1) *v = x;

	if (!hcns(n_alloc)(r, ndigits)) {
		HC_FATAL("n_alloc(%p, %i) failed", r, ndigits);
	}
	r->len = ndigits;

	for (i=ndigits-1; i>=0; i--) {
		r->d[i] = v[0] << 8 | v[1];
		v += 2;
	}
}

void hcns(n_load_hex)(struct hcns(n) *r, char *hex, int n)
{
	int nbytes = (n + 1) / 2;
	int i, ndigits = (nbytes + (HC_HALF_BYTES-1)) / HC_HALF_BYTES;
	hcns(u1) *v;

	assert(n > 0);

	if (!hcns(n_alloc)(r, ndigits)) {
		HC_FATAL("n_alloc(%p, %i) failed", r, ndigits);
	}
	r->len = ndigits;

	i = ndigits - 1;
	v = (hcns(u1)*) hex;

	switch (n % (HC_HALF_BYTES * 2)) {
	case 3:
		r->d[i--] =
			HC_HEX_VALUE(v[0]) << 8 |
			HC_HEX_VALUE(v[1]) << 4 |
			HC_HEX_VALUE(v[2]);
		n -= HC_HALF_BYTES * 2 - 1;
		v += HC_HALF_BYTES * 2 - 1;
		break;
	case 2:
		r->d[i--] =
			HC_HEX_VALUE(v[0]) << 4 |
			HC_HEX_VALUE(v[1]);
		n -= HC_HALF_BYTES * 2 - 2;
		v += HC_HALF_BYTES * 2 - 2;
		break;
	case 1:
		r->d[i--] = HC_HEX_VALUE(v[0]);
		n -= HC_HALF_BYTES * 2 - 3;
		v += HC_HALF_BYTES * 2 - 3;
		break;
	}

	while (n >= HC_HALF_BYTES * 2) {
		r->d[i--] =
			HC_HEX_VALUE(v[0]) << 12 |
			HC_HEX_VALUE(v[1]) << 8 |
			HC_HEX_VALUE(v[2]) << 4 |
			HC_HEX_VALUE(v[3]);
		n -= HC_HALF_BYTES * 2;
		v += HC_HALF_BYTES * 2;
	}

	assert(n == 0 && i == -1);
}

void hcns(n_load_hexz)(struct hcns(n) *r, char *hex)
{
	return hcns(n_load_hex)(r, hex, hcns(slen)(hex));
}

void hcns(n_as_hex)(struct hcns(n) *n, struct hcns(s) *s)
{
	int i, ndigits = n->len;
	int s_start_len = s->len;

	if (n->d == NULL || n->len == 0) {
		/* uninitialized
		 */
		hcns(s_catn)(s, "0", 1);
		return;
	}

	hcns(s_alloc)(s, s->len + n->len * (HC_HALF_BYTES * 2)); /* each byte has 2 hex digits */

	i = ndigits - 1;

	/* skip leading zeroes
	 */

	while (i>=0) {
		char t[4];
		hcns(h) d = n->d[i--];

		if (d) {
			t[0] = HC_HEX_DIGIT(d >> 12);
			t[1] = HC_HEX_DIGIT(d >> 8);
			t[2] = HC_HEX_DIGIT(d >> 4);
			t[3] = HC_HEX_DIGIT(d);

			int j = 0;

			while (j < 4 && t[j] == '0') j++;

			hcns(s_catn)(s, t + j, 4 - j);

			break;
		}
	}

	/* output remaining digits
	 */

	while (i>=0) {
		char t[4];
		hcns(h) d = n->d[i--];

		t[0] = HC_HEX_DIGIT(d >> 12);
		t[1] = HC_HEX_DIGIT(d >> 8);
		t[2] = HC_HEX_DIGIT(d >> 4);
		t[3] = HC_HEX_DIGIT(d);

		hcns(s_catn)(s, t, 4);
	}

	if (s_start_len == s->len) {
		hcns(s_catn)(s, "0", 1);
	}
}

#else
#error not implemented
#endif
