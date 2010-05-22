#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/str.h"
#include "higherc/alloc.h"
#include "higherc/bytewise.h"
#include "higherc/s.h"
#include "higherc/n.h"

#define D_EXP  HC_HALF_BITS  /* exponent of 2 */
#define D_MASK HC_HALF_MASK

#define DLOW HC_LOW
#define DHIGH HC_HIGH
#define DLOW2HIGH HC_TO_HIGH
#define DDIGIT HC_F

#define ZERO_DIGITS HC_ZERO_DIGITS 
#define MOVE_DIGITS HC_MOVE_DIGITS 

#if HC_HALF_BITS == 16
#define HALF_OFFENSE 0xdeadU  /* ``offensive programming'' */ 
#define OFFENSE 0xdeadbeefU   /* ``offensive programming'' */ 
#else
#error not implemented
#endif

#define B36_2 1296  /* pow(36, 2) */
#define B36_3 46656  /* pow(36, 3) */

#if HC_HALF_BITS == 32
#define B36_4 1679616
#define B36_5 60466176
#define B36_6 2176782336 /* 6 digits after each 2176782336 division */
#endif

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
	x->d[x->len] = HALF_OFFENSE;   /* ``offensive programming'' */ 
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
	n->len = n->d[1] ? 2 : 1;
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
	int i, nbytes, ndigits;
	hcns(u1) *v;

	assert(hex != NULL);
	assert(n > 0);

	/* strip leading zeros
	 */
	while (n > 1 && *hex == '0') {
		hex++;
		n--;
	}

	/*
	 */

	nbytes = (n + 1) / 2;
	ndigits = (nbytes + (HC_HALF_BYTES-1)) / HC_HALF_BYTES;

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
			int j;

			t[0] = HC_HEX_DIGIT(d >> 12);
			t[1] = HC_HEX_DIGIT(d >> 8);
			t[2] = HC_HEX_DIGIT(d >> 4);
			t[3] = HC_HEX_DIGIT(d);

			j = 0;
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

void hcns(n_as_dec)(struct hcns(n) *n, struct hcns(s) *s)
{
	HC_DEF_N(q);
	hcns(h) r = HC_H(0);
	int s_len0 = s->len;

	if (HC_IS_ZERO(n)) {
		hcns(s_catn)(s, "0", 1);
		return;
	}

	hcns(n_alloc)(q, n->len);
	HC_MOVE_DIGITS(q->d, n->d, n->len);
	q->len = n->len;

	while (!HC_IS_ZERO(q)) {
		q->len = D_div(q->d, q->len, 10000, q->d, &r);

		hcns(s_alloc)(s, s->len + 4);

		s->s[s->len++] = HC_DEC_DIGIT(r % 10);
		s->s[s->len++] = HC_DEC_DIGIT((r / 10) % 10);
		s->s[s->len++] = HC_DEC_DIGIT(((r / 10) / 10) % 10);
		s->s[s->len++] = HC_DEC_DIGIT((((r / 10) / 10) / 10) % 10);
	}

	while (s->s[s->len - 1] == '0') s->len--;

	hcns(brev)(s->s + s_len0, s->len - s_len0);

	hcns(n_free)(q);
}

void hcns(n_as_base36)(struct hcns(n) *n, struct hcns(s) *s)
{
	HC_DEF_N(q);
	hcns(h) r = HC_H(0);
	int s_len0 = s->len;

	if (HC_IS_ZERO(n)) {
		hcns(s_catn)(s, "0", 1);
		return;
	}

	hcns(n_alloc)(q, n->len);
	HC_MOVE_DIGITS(q->d, n->d, n->len);
	q->len = n->len;

	while (!HC_IS_ZERO(q)) {
		q->len = D_div(q->d, q->len, B36_3, q->d, &r);

		hcns(s_alloc)(s, s->len + 3);

		s->s[s->len++] = HC_BASE36_DIGIT(r % 36);
		s->s[s->len++] = HC_BASE36_DIGIT((r / 36) % 36);
		s->s[s->len++] = HC_BASE36_DIGIT(((r / 36) / 36) % 36);
	}

	while (s->s[s->len - 1] == '0') s->len--;

	hcns(brev)(s->s + s_len0, s->len - s_len0);

	hcns(n_free)(q);
}

#else
#error not implemented
#endif

int hcns(n_cmp_hex)(struct hcns(n) *v, char *hex, int n)
{
	struct hcns(s) s = HC_NULL_S;
	int r;

	assert(hex != NULL);
	assert(n > 0);

	/* strip leading zeros
	 */
	while (n > 1 && *hex == '0') {
		hex++;
		n--;
	}

	if (*hex == 0) hex--;

	hcns(n_as_hex)(v, &s);
	HC_SAFE_CSTR(&s);

	r = hcns(s_bdiff)(&s, hex, n);

	hcns(s_free)(&s);

	return r;
}

int hcns(n_cmp_hexz)(struct hcns(n) *v, char *hex)
{
	return hcns(n_cmp_hex)(v, hex, hcns(slen(hex)));
}

/**************************************
 */

#define DMULc(a,b,c,p) do {				\
		hcns(f) _t = ((hcns(f))(a))*(b) + (c);	\
		p = DLOW(_t);				\
		c = DHIGH(_t);				\
	} while(0)
#define DMUL(a,b,c1,c0) do {				\
		hcns(f) _t = ((hcns(f))(a))*(b);	\
		c0 = DLOW(_t);				\
		c1 = DHIGH(_t);				\
	} while(0)

#define DDIV(a1,a0,b,q) do {			\
		hcns(f) _t = DDIGIT((a1),(a0));	\
		q = _t / (b);			\
	} while(0)

#define DDIV2(a1,a0,b1,b0,q) do {		\
		hcns(f) _t = DDIGIT((a1),(a0));	\
		q = _t / DDIGIT((b1),(b0));	\
	} while(0)

#define DREM(a1,a0,b,r) do {			\
		hcns(f) _t = DDIGIT((a1),(a0));	\
		r = _t % (b);			\
	} while(0)

/* add a and b with carry in + out */
#define DSUMc(a,b,c,s) do {			\
		hcns(h) ___cr = (c);		\
		hcns(h) ___xr = (a)+(___cr);	\
		hcns(h) ___yr = (b);		\
		___cr = (___xr < ___cr);	\
		___xr = ___yr + ___xr;		\
		___cr += (___xr < ___yr);	\
		s = ___xr;			\
		c = ___cr;			\
	}  while(0)

/* add a and b with carry out */
#define DSUM(a,b,c,s) do {			\
		hcns(h) ___xr = (a);		\
		hcns(h) ___yr = (b);		\
		___xr = ___yr + ___xr;		\
		s = ___xr;			\
		c = (___xr < ___yr);		\
	}  while(0)

#define DSUBb(a,b,r,d) do {			\
		hcns(h) ___cr = (r);		\
		hcns(h) ___xr = (a);		\
		hcns(h) ___yr = (b)+___cr;	\
		___cr = (___yr < ___cr);	\
		___yr = ___xr - ___yr;		\
		___cr += (___yr > ___xr);	\
		d = ___yr;			\
		r = ___cr;			\
	} while(0)

#define DSUB(a,b,r,d) do {			\
		hcns(h) ___xr = (a);		\
		hcns(h) ___yr = (b);		\
		___yr = ___xr - ___yr;		\
		r = (___yr > ___xr);		\
		d = ___yr;			\
	} while(0)

/* type a constant as a hcns(h) - to get shifts correct */
#define DCONST(n) ((hcns(h))(n))

/*
** compare two number vectors
*/
int I_comp(hcns(h) *x, int xl, hcns(h) *y, int yl)
{
	if (xl < yl)
		return -1;
	else if (xl > yl)
		return 1;
	else {
		if (x == y)
			return 0;
		x += (xl-1);
		y += (yl-1);
		while((xl > 0) && (*x == *y)) {
			x--;
			y--;
			xl--;
		}
		if (xl == 0)
			return 0;
		return (*x < *y) ? -1 : 1;
	}
}

/*
** Add digits in x and y and store them in r
** assumption: (xl >= yl)
*/
static int I_add(hcns(h) *x, int xl, hcns(h) *y, int yl, hcns(h) *r)
{
	int sz = xl;
	register hcns(h) yr, xr;
	register hcns(h) c = 0;

	assert(xl >= yl);

	xl -= yl;
	do {
		xr = *x++ + c;
		yr = *y++;
		c = (xr < c);
		xr = yr + xr;
		c += (xr < yr);
		*r++ = xr;
	} while(--yl);

	while(xl--) {
		xr = *x++ + c;
		c = (xr < c);
		*r++ = xr;
	}
	if (c) {
		*r = 1;
		return sz+1;
	}
	return sz;
}
/*
** Add a digits in v1 and store result in vr
*/
static int D_add(hcns(h) *x, int xl, hcns(h) c, hcns(h) *r)
{
	int sz = xl;
	register hcns(h) xr;

	while(xl--) {
		xr = *x++ + c;
		c = (xr < c);
		*r++ = xr;
	}
	if (c) {
		*r = 1;
		return sz+1;
	}
	return sz;
}

/*
** Subtract digits v2 from v1 and store result in v3
** Assert  I_comp(x, xl, y, yl) >= 0
**
*/
static int I_sub(hcns(h) *x, int xl, hcns(h) *y, int yl, hcns(h) *r)
{
	hcns(h) *r0 = r;
	register hcns(h) yr, xr;
	register hcns(h) c = 0;

	assert(I_comp(x, xl, y, yl) >= 0);

	xl -= yl;
	do {
		yr = *y++ + c;
		xr = *x++;
		c = (yr < c);
		yr = xr - yr;
		c += (yr > xr);
		*r++ = yr;
	} while(--yl);

	while(xl--) {
		xr = *x++;
		yr = xr - c;
		c = (yr > xr);
		*r++ = yr;
	}
	do {
		r--;
	} while(*r == 0 && r != r0);

	return (r - r0) + 1;
}

/*
** Subtract digit d from v1 and store result in vr
*/
static int D_sub(hcns(h) *x, int xl, hcns(h) c, hcns(h) *r)
{
	hcns(h) *r0 = r;
	register hcns(h) yr, xr;

	assert(I_comp(x, xl, x, 1) >= 0);

	while(xl--) {
		xr = *x++;
		yr = xr - c;
		c = (yr > xr);
		*r++ = yr;
	}
	do {
		r--;
	} while(*r == 0 && r != r0);

	return (r - r0) + 1;
}

/*
** subtract Z000...0 - y and store result in r, return new size
*/
static int Z_sub(hcns(h) *y, int yl, hcns(h) *r)
{
	hcns(h) *r0 = r;
	register hcns(h) yr;
	register hcns(h) c = 0;

	while(yl--) {
		yr = *y++ + c;
		c = (yr < c);
		yr = 0 - yr;
		c += (yr > 0);
		*r++ = yr;
	}
	do {
		r--;
	} while(*r == 0 && r != r0);
	return (r - r0) + 1;
}

/*
** Multiply digits in x with digits in y and store in r
** Assumption: digits in r must be 0 (up to the size of greater operand)
*/
int I_mul(hcns(h) *x, int xl, hcns(h) *y, int yl, hcns(h) *r)
{
	hcns(h) *r0 = r;
	hcns(h) *rt = r;

	while(xl--) {
		hcns(h) cp = 0;
		hcns(h) c = 0;
		int n = yl;
		hcns(h) *yt = y;
		hcns(h) d;
		hcns(h) p;

		d = *x; 
		x++;
		rt = r;

		switch(d) {
		case 0:
			rt = rt + n;
			break;
		case 1:
			while(n--) {
				DSUMc(*yt, *rt, c, p);
				*rt++ = p;
				yt++;
			}
			break;
		case 2:
			while(n--) {
				p = *yt;
				DSUMc(p, p, cp, p);
				DSUMc(p, *rt, c, p);
				*rt++ = p;
				yt++;
			}
			break;
		default:
			while(n--) {
				DMULc(d, *yt, cp, p);
				DSUMc(p, *rt, c, p);
				*rt++ = p;
				yt++;
			}
			break;
		}
		*rt = c + cp;
		r++;
	}
	if (*rt == 0)
		return (rt - r0);
	else
		return (rt - r0) + 1;
}

/*
** Square digits in x store in r (x & r may point into a common area)
** Assumption: x is destroyed if common area and digits in r are zero
**             to the size of xl+1
*/

static int I_sqr(hcns(h) *x, int xl, hcns(h) *r)
{
	hcns(h) d_next = *x;
	hcns(h) d;
	hcns(h) *r0 = r;
	hcns(h) *s = r;

	if ((r + xl) == x)	/* "Inline" operation */
		*x = 0;
	x++;
	
	while(xl--) {
		hcns(h) *y = x;
		hcns(h) y_0 = 0, y_1 = 0, y_2 = 0, y_3 = 0;
		hcns(h) b0, b1;
		hcns(h) z0, z1, z2;
		hcns(h) t;
		int y_l = xl;
		
		s = r;
		d = d_next;
		d_next = *x; 
		x++;

		DMUL(d, d, b1, b0);
		DSUMc(*s, b0, y_3, t);
		*s++ = t;
		z1 = b1;
		while(y_l--) {
			DMUL(d, *y, b1, b0);
			y++;
			DSUMc(b0, b0, y_0, z0);
			DSUMc(z0, z1, y_2, z2);
			DSUMc(*s, z2, y_3, t);
			*s++ = t;
			DSUMc(b1, b1, y_1, z1);
		}
		z0 = y_0;
		DSUMc(z0, z1, y_2, z2);
		DSUMc(*s, z2, y_3, t);
		*s = t;
		if (xl != 0) {
			s++;
			t = (y_1+y_2+y_3);
			*s = t;
			r += 2;
		}
		else {
			assert((y_1+y_2+y_3) == 0);
		}
	}
	if (*s == 0)
		return (s - r0);
	else
		return (s - r0) + 1;
}

/*
 * Multiply digits d with digits in x and store in r
 *
 * r[xl+1] = x[xl] * d
 * return r length
 */
int D_mul(hcns(h) *x, int xl, hcns(h) d, hcns(h) *r)
{
	hcns(h) c = 0;
	int rl = xl;
	hcns(h) p;

	switch(d) {
	case 0:
		ZERO_DIGITS(r, 1);
		return 1;
	case 1:
		if (x != r)
			MOVE_DIGITS(r, x, xl);
		return xl;
	case 2:
		while(xl--) {
			p = *x;
			DSUMc(p, p, c, p);
			*r++ = p;
			x++;
		}
		break;
	default:
		while(xl--) {
			DMULc(d, *x, c, p);
			*r++ = p;
			x++;
		}
		break;
	}
	if (c == 0)
		return rl;
	*r = c;
	return rl+1;
}

/*
** Multiply and subtract
** calculate r(i) = x(i) - d*y(i)
** assumption: xl = yl || xl == yl+1
**
** Return size of r
** 0 means borrow
*/
static int D_mulsub(hcns(h) *x, int xl, hcns(h) d,
			   hcns(h) *y, int yl, hcns(h) *r)
{
	hcns(h) c = 0;
	hcns(h) b = 0;
	hcns(h) c0;
	hcns(h) *r0 = r;
	hcns(h) s;

	assert(xl == yl || xl == yl+1);

	xl -= yl;
	while(yl--) {
		DMULc(d, *y, c, c0);
		DSUBb(*x, c0, b, s);
		*r++ = s;
		x++;
		y++;
	}
	if (xl == 0) {
		if (c != 0 || b != 0)
			return 0;
	}
	else {			/* xl == 1 */
		DSUBb(*x, c, b, s);
		*r++ = s;
	}
	if (b != 0) return 0;

	do {
		r--;
	} while(*r == 0 && r != r0);
	return (r - r0) + 1;
}

/*
** Divide digits in x with a digit,
** quotient is returned in q and remainder digit in r
** x and q may be equal
*/
int D_div(hcns(h) *x, int xl, hcns(h) d, hcns(h) *q, hcns(h) *r)
{
	hcns(h) *xp = x + (xl-1);
	hcns(h) *qp = q + (xl-1);
	int qsz = xl;
	hcns(h) a1;
	
	a1 = *xp; 
	xp--;

	if (d > a1) {
		if (xl == 1) {
			*r = a1;
			*qp = 0;
			return 1;
		}
		qsz--;
		qp--;
	}

	do {
		hcns(h) q0, a0, b1, b0, b;

		if (d > a1) {
			a0 = *xp; 
			xp--;
		}
		else {
			a0 = a1; a1 = 0;
		}
		DDIV(a1, a0, d, q0);
		DMUL(d, q0, b1, b0);
		DSUB(a0,b0, b, a1);
		*qp = q0;
		qp--;
	} while (xp >= x);

	*r = a1;
	return qsz;
}

/*
** Divide digits in x with digits in y and return qutient in q
** and remainder in r
** assume that integer(x) > integer(y)
** Return remainder in x (length int rl)
** Return quotient size
*/

int I_div(hcns(h) *x, int xl, hcns(h) *y, int yl,
		 hcns(h) *q, hcns(h) *r, int *rlp)
{
	hcns(h) *rp;
	hcns(h) *qp;
	hcns(h) b1 = y[yl-1];
	hcns(h) b2 = y[yl-2];
	hcns(h) a1;
	hcns(h) a2;
	int r_signed = 0;
	int ql;
	int rl;

	if (x != r)
		MOVE_DIGITS(r, x, xl);
	rp = r + (xl-yl);
	rl = xl;
	
	ZERO_DIGITS(q, xl-yl+1);
	qp = q + (xl-yl);
	ql = 0;
	
	/* Adjust length */
	a1 = rp[yl-1];
	a2 = rp[yl-2];
	if (b1 < a1 || (b1 == a1 && b2 <= a2))
		ql = 1;

	do {
		hcns(h) q0;
		int nsz = yl;
		int nnsz;

		a1 = rp[yl-1];
		a2 = rp[yl-2];

		if (b1 < a1)
			DDIV2(a1,a2,b1,b2,q0);
		else if (b1 > a1) {
			DDIV(a1,a2,b1,q0);
			nsz++;
			rp--;
			qp--;
			ql++;
		}
		else {			/* (b1 == a1) */
			if (b2 <= a2)
				q0 = 1;
			else {
				q0 = D_MASK;
				nsz++;
				rp--;
				qp--;
				ql++;
			}
		}

		if (r_signed)
			ql = D_sub(qp, ql, q0, qp);
		else
			ql = D_add(qp, ql, q0, qp);

		if ((nnsz = D_mulsub(rp, nsz, q0, y, yl, rp)) == 0) {
			nnsz = Z_sub(r, rl, r);
			if (nsz > (rl-nnsz))
				nnsz = nsz - (rl-nnsz);
			else
				nnsz = 1;
			r_signed = !r_signed;
		}
		
		if ((nnsz == 1) && (*rp == 0))
			nnsz = 0;
		rp = rp - (yl-nnsz);
		rl -= (nsz-nnsz);
		qp = qp - (yl-nnsz);
		ql += (yl-nnsz);
	} while (I_comp(r, rl, y, yl) >= 0);

	ql -= (q - qp);
	qp = q;

	if (rl == 0)
		rl = 1;

	while(rl > 1 && r[rl-1] == 0) /* Remove "trailing zeroes" */
		--rl;

	if (r_signed && (rl > 1 || *r != 0)) {
		rl = I_sub(y, yl, r, rl, r);
		ql = D_sub(qp, ql, 1, qp);
	}

	*rlp = rl;
	return ql;
}

/*
** Remainder of digits in x and a digit d
*/
static hcns(h) D_rem(hcns(h) *x, int xl, hcns(h) d)
{
	hcns(h) rem = 0;

	x += (xl-1);
	do {
		if (rem != 0)
			DREM(rem, *x, d, rem);
		else
			DREM(0, *x, d, rem);
		x--;
		xl--;
	} while(xl > 0);
	return rem;
}

/*
** Remainder of x and y
**
** Assumtions: xl >= yl, yl > 1
**			   r must contain at least xl number of digits
*/
static int I_rem(hcns(h) *x, int xl, hcns(h) *y, int yl, hcns(h) *r)
{
	hcns(h) *rp;
	hcns(h) b1 = y[yl-1];
	hcns(h) b2 = y[yl-2];
	hcns(h) a1;
	hcns(h) a2;
	int r_signed = 0;
	int rl;
	
	if (x != r)
		MOVE_DIGITS(r, x, xl);
	rp = r + (xl-yl);
	rl = xl;

	do {
		hcns(h) q0;
		int nsz = yl;
		int nnsz;
		
		a1 = rp[yl-1];
		a2 = rp[yl-2];

		if (b1 < a1)
			DDIV2(a1,a2,b1,b2,q0);
		else if (b1 > a1) {
			DDIV(a1,a2,b1,q0);
			nsz++;
			rp--;
		}
		else {			/* (b1 == a1) */
			if (b2 <= a2)
				q0 = 1;
			else {
				q0 = D_MASK;
				nsz++;
				rp--;
			}
		}

		if ((nnsz = D_mulsub(rp, nsz, q0, y, yl, rp)) == 0) {
			nnsz = Z_sub(r, rl, r);
			if (nsz > (rl-nnsz))
				nnsz = nsz - (rl-nnsz);
			else
				nnsz = 1;
			r_signed = !r_signed;
		}

		if (nnsz == 1 && *rp == 0)
			nnsz = 0;

		rp = rp - (yl-nnsz);
		rl -= (nsz-nnsz);
	} while (I_comp(r, rl, y, yl) >= 0);

	if (rl == 0)
		rl = 1;

	while(rl > 1 && r[rl-1] == 0) /* Remove "trailing zeroes" */
		--rl;

	if (r_signed && (rl > 1 || *r != 0))
		rl = I_sub(y, yl, r, rl, r);
	return rl;
}

/*
** Remove trailing digits from bitwise operations
*/
static int I_btrail(hcns(h) *r0, hcns(h) *r, short sign)
{
	/* convert negative numbers to one complement */
	if (sign) {
		int rl;
		hcns(h) d;

		/* 1 remove all 0xffff words */
		do {
			r--;
		} while(((d = *r) == D_MASK) && (r != r0));

		/* 2 complement high digit */
		if (d == D_MASK)
			*r = 0;
		else {
			hcns(h) prev_mask = 0;
			hcns(h) mask = (DCONST(1)<<(D_EXP-1));

			while((d & mask) == mask) {
				prev_mask = mask;
				mask = (prev_mask >> 1) | (DCONST(1)<<(D_EXP-1));
			}
			*r = ~d & ~prev_mask;
		}
		rl = (r - r0) + 1;
		while(r != r0) {
			r--;
			*r = ~*r;
		}
		return D_add(r0, rl, 1, r0);
	}

	do {
		r--;
	} while(*r == 0 && r != r0);
	return (r - r0) + 1;
}

/* 
** Bitwise and
*/
static int I_band(hcns(h) *x, int xl, short xsgn,
			 hcns(h) *y, int yl, short ysgn, hcns(h) *r)
{
	hcns(h) *r0 = r;
	short sign = xsgn && ysgn;

	assert(xl >= yl);

	xl -= yl;

	if (!xsgn) {
		if (!ysgn) {
			while(yl--)
				*r++ = *x++ & *y++;
		}
		else {
			hcns(h) b;
			hcns(h) c;

			DSUB(*y,1,b,c);
			*r++ = *x++ & ~c;
			y++;
			yl--;
			while(yl--) {
				DSUBb(*y,0,b,c);
				*r++ = *x++ & ~c;
				y++;
			}
			while (xl--) {
				*r++ = *x++;
			}
		}
	}
	else {
		if (!ysgn) {
			hcns(h) b;
			hcns(h) c;

			DSUB(*x,1,b,c);
			*r = ~c & *y;
			x++; y++; r++;
			yl--;
			while(yl--) {
				DSUBb(*x,0,b,c);
				*r++ = ~c & *y++;
				x++;
			}
		}
		else {
			hcns(h) b1, b2;
			hcns(h) c1, c2;

			DSUB(*x,1,b1,c1);
			DSUB(*y,1,b2,c2);
			*r++ = ~c1 & ~c2;
			x++; y++;
			yl--;
			while(yl--) {
				DSUBb(*x,0,b1,c1);
				DSUBb(*y,0,b2,c2);
				*r++ = ~c1 & ~c2;
				x++; y++;
			}
			while(xl--)
				*r++ = ~*x++;
		}
	}
	return I_btrail(r0, r, sign);
}

/* 
 * Bitwise 'or'.
 */
static int
I_bor(hcns(h) *x, int xl, short xsgn, hcns(h) *y,
      int yl, short ysgn, hcns(h) *r)
{
	hcns(h) *r0 = r;
	short sign = xsgn || ysgn;

	assert(xl >= yl);

	xl -= yl;

	if (!xsgn) {
		if (!ysgn) {
			while(yl--)
				*r++ = *x++ | *y++;
			while(xl--)
				*r++ = *x++;
		}
		else {
			hcns(h) b;
			hcns(h) c;

			DSUB(*y,1,b,c);
			*r++ = *x++ | ~c;
			y++;
			yl--;
			while(yl--) {
				DSUBb(*y,0,b,c);
				*r++ = *x++ | ~c;
				y++;
			}
		}
	}
	else {
		if (!ysgn) {
			hcns(h) b;
			hcns(h) c;

			DSUB(*x,1,b,c);
			*r++ = ~c | *y++;
			x++;
			yl--;
			while(yl--) {
				DSUBb(*x,0,b,c);
				*r++ = ~c | *y++;
				x++;
			}
			while(xl--) {
				DSUBb(*x,0,b,c);
				*r++ = ~c;
				x++;
			}
		}
		else {
			hcns(h) b1, b2;
			hcns(h) c1, c2;

			DSUB(*x,1,b1,c1);
			DSUB(*y,1,b2,c2);
			*r++ = ~c1 | ~c2;
			x++; y++;
			yl--;
			while(yl--) {
				DSUBb(*x,0,b1,c1);
				DSUBb(*y,0,b2,c2);
				*r++ = ~c1 | ~c2;
				x++; y++;
			}
		}
	}
	return I_btrail(r0, r, sign);
}

/* 
** Bitwise xor
*/
static int I_bxor(hcns(h) *x, int xl, short xsgn,
			 hcns(h) *y, int yl, short ysgn, hcns(h) *r)
{
	hcns(h) *r0 = r;
	short sign = xsgn != ysgn;

	assert(xl >= yl);

	xl -= yl;

	if (!xsgn) {
		if (!ysgn) {
			while(yl--)
				*r++ = *x++ ^ *y++;
			while(xl--)
				*r++ = *x++;
		}
		else {
			hcns(h) b;
			hcns(h) c;

			DSUB(*y,1,b,c);
			*r++ = *x++ ^ ~c;
			y++;
			yl--;
			while(yl--) {
				DSUBb(*y,0,b,c);
				*r++ = *x++ ^ ~c;
				y++;
			}
			while(xl--)
				*r++ = ~*x++;
		}
	}
	else {
		if (!ysgn) {
			hcns(h) b;
			hcns(h) c;

			DSUB(*x,1,b,c);
			*r++ = ~c ^ *y++;
			x++;
			yl--;
			while(yl--) {
				DSUBb(*x,0,b,c);
				*r++ = ~c ^ *y++;
				x++;
			}
			while(xl--)
				*r++ = ~*x++;
		}
		else {
			hcns(h) b1, b2;
			hcns(h) c1, c2;

			DSUB(*x,1,b1,c1);
			DSUB(*y,1,b2,c2);
			*r++ = ~c1 ^ ~c2;
			x++; y++;
			yl--;
			while(yl--) {
				DSUBb(*x,0,b1,c1);
				DSUBb(*y,0,b2,c2);
				*r++ = ~c1 ^ ~c2;
				x++; y++;
			}
			while(xl--) {
				*r++ = *x++;
			}
		}
	}
	return I_btrail(r0, r, sign);
}

/*
** Bitwise not  simulated as
** bnot -X  == (X - 1)
** bnot +X  == -(X + 1)
*/
static int I_bnot(hcns(h) *x, int xl, short xsgn, hcns(h) *r)
{
	if (xsgn)
		return D_add(x, xl, 1, r);
	else
		return D_sub(x, xl, 1, r);
}

/*
** Arithmetic left shift or right
*/
static int I_lshift(hcns(h) *x, int xl, int y, 
			   short sign, hcns(h) *r)
{
	if (y == 0) {
		MOVE_DIGITS(r, x, xl);
		return xl;
	} else if (xl == 1 && *x == 0) {
		*r = 0;
		return 1;
	} else {
		long ay = (y < 0) ? -y : y;
		int bw = ay / D_EXP;
		int sw = ay % D_EXP;
		int rl;
		hcns(h) a1=0;
		hcns(h) a0=0;

		if (y > 0) {		/* shift left */
			rl = xl + bw + 1;

			while(bw--)
				*r++ = 0;
			if (sw) {  // NOTE! x >> 32 is not = 0!
				while(xl--) {
					a0 = (*x << sw) | a1;
					a1 = (*x >> (D_EXP - sw));
					*r++ = a0;
					x++;
				}
			}
			else {
				while(xl--) {
					*r++ = *x++;
				}
			}
			if (a1 == 0)
				return rl-1;
			*r = a1;
			return rl;
		} else {			/* shift right */
			hcns(h) *r0 = r;
			int add_one = 0;

			if (xl <= bw) {
				if (sign)
					*r = 1;
				else
					*r = 0;
				return 1;
			}

			if (sign) {
				int zl = bw;
				hcns(h) *z = x;

				while(zl--) {
					if (*z != 0) {
						add_one = 1;
						break;
					}
					z++;
				}
			}

			rl = xl - bw;
			x += (xl-1);
			r += (rl-1);
			xl -= bw;
			if (sw) { // NOTE! x >> 32 is not = 0!
				while(xl--) {
					a1 = (*x >> sw) | a0;
					a0 = (*x << (D_EXP-sw));
					*r-- = a1;
					x--;
				}
			} else {
				while(xl--) {
					*r-- = *x--;
				}
			}

			if (sign && (a0 != 0))
				add_one = 1;

			if (r[rl] == 0) {
				if (rl == 1) {
					if (sign)
						r[1] = 1;
					return 1;
				}
				rl--;
			}
			if (add_one)
				return D_add(r0, rl, 1, r0);
			return rl;
		}
	}
}

/*
** Return log(x)/log(2)
*/
static int I_lg(hcns(h) *x, int xl)
{
	int sz = xl - 1;
	hcns(h) d = x[sz];

	sz *= D_EXP;
	while(d != 0) {
		d >>= 1;
		sz++;
	}
	return sz - 1;
}
