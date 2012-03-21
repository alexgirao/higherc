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

/*
 * This source code has chunks of code from erlang/otp project version
 * R14B01, original code is at
 * 3rd/otp_src_R14B01/erts/emulator/beam/big.{c,h} and is licensed
 * under ERLANG PUBLIC LICENSE Version 1.1 see file at
 * 3rd/otp_src_R14B01/EPLICENCE for complete description.
 */

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

void n_alloc(struct n *x, int n)
{
	if (x->d) {
		if (n > x->a) {
			int i = n;
			void *p = alloc_re(x->d, x->len * sizeof(h), i * sizeof(h));
			if (p) {
				x->a = i;
				x->d = p;
				return;
			}
			HC_FATAL("memory allocation failed"); /* it's better be safer than sorry */
		}
		return;
	}
	HC_ALLOC(x->d, n * sizeof(h));
	x->a = n;
	x->len = 0;
}

void n_free(struct n *x)
{
	if (x->d) {
		void *p = x->d;
		x->d = NULL;
		x->len = 0;
		x->a = 0;
		HC_FREE(p);
	}
}

/* copy
 */

void n_copyn(struct n *x, const h *d, int n)
{
	n_alloc(x, (n + 1) * sizeof(h));
	b_copyl(x->d, n * sizeof(h), d);
	x->len = n;
	x->d[x->len] = HALF_OFFENSE;   /* ``offensive programming'' */ 
}

void n_copy(struct n *to, const struct n *from)
{
	n_copyn(to, from->d, from->len);
}

/*
 */

#if (HC_HALF_BYTES == 2)

void n_set_u4(struct n *n, uint32_t v)
{
	n_alloc(n, 2);
	n->d[0] = HC_LOW(v);
	n->d[1] = HC_HIGH(v);
	n->len = n->d[1] ? 2 : 1;
}

void n_load_be1(struct n *r, void *x, int len)
{
	int i, ndigits = len / HC_HALF_BYTES;
	uint8_t *v = x;

	n_alloc(r, ndigits);
	r->len = ndigits;

	for (i=ndigits-1; i>=0; i--) {
		r->d[i] = v[0] << 8 | v[1];
		v += 2;
	}
}

void n_load_hex(struct n *r, char *hex, int n)
{
	int i, nbytes, ndigits;
	uint8_t *v;

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

	n_alloc(r, ndigits);
	r->len = ndigits;

	i = ndigits - 1;
	v = (uint8_t*) hex;

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

void n_load_hexz(struct n *r, char *hex)
{
	return n_load_hex(r, hex, slen(hex));
}

int n_as_hex(struct n *n, HC_ST_S *s)
{
	int i, ndigits = n->len;
	int s_start_len = s->len;

	if (HC_IS_ZERO(n)) {
		s_catc(s, '0');
		return 1;
	}

	s_alloc(s, s->len + n->len * (HC_HALF_BYTES * 2)); /* each byte has 2 hex digits */

	i = ndigits - 1;

	/* skip leading zeroes
	 */

	while (i>=0) {
		char t[4];
		h d = n->d[i--];

		if (d) {
			int j;

			t[0] = HC_HEX_DIGIT((d >> 12) & 0xf);
			t[1] = HC_HEX_DIGIT((d >> 8) & 0xf);
			t[2] = HC_HEX_DIGIT((d >> 4) & 0xf);
			t[3] = HC_HEX_DIGIT(d & 0xf);

			j = 0;
			while (j < 4 && t[j] == '0') j++;

			s_catn(s, t + j, 4 - j);

			break;
		}
	}

	/* output remaining digits
	 */

	while (i>=0) {
		char t[4];
		h d = n->d[i--];

		t[0] = HC_HEX_DIGIT((d >> 12) & 0xf);
		t[1] = HC_HEX_DIGIT((d >> 8) & 0xf);
		t[2] = HC_HEX_DIGIT((d >> 4) & 0xf);
		t[3] = HC_HEX_DIGIT(d & 0xf);

		s_catn(s, t, 4);
	}

	if (s_start_len == s->len) {
		s_catc(s, '0');
		return 1;
	}

	return s->len - s_start_len;
}

int n_as_dec(struct n *n, HC_ST_S *s)
{
	HC_DEF_N(q);
	h r = HC_H(0);
	int s_len0 = s->len;

	if (HC_IS_ZERO(n)) {
		s_catc(s, '0');
		return 1;
	}

	n_alloc(q, n->len);
	HC_MOVE_DIGITS(q->d, n->d, n->len);
	q->len = n->len;

	while (!HC_IS_ZERO(q)) {
		q->len = D_div(q->d, q->len, 10000, q->d, &r);

		s_alloc(s, s->len + 4);

		s->s[s->len++] = HC_DEC_DIGIT(r % 10);
		s->s[s->len++] = HC_DEC_DIGIT((r / 10) % 10);
		s->s[s->len++] = HC_DEC_DIGIT(((r / 10) / 10) % 10);
		s->s[s->len++] = HC_DEC_DIGIT((((r / 10) / 10) / 10) % 10);
	}

	while (s->s[s->len - 1] == '0') s->len--;

	b_rev(s->s + s_len0, s->len - s_len0);

	n_free(q);

	return s->len - s_len0;
}

int n_as_base36(struct n *n, HC_ST_S *s)
{
	HC_DEF_N(q);
	h r = HC_H(0);
	int s_len0 = s->len;

	if (HC_IS_ZERO(n)) {
		s_catc(s, '0');
		return 1;
	}

	n_alloc(q, n->len);
	HC_MOVE_DIGITS(q->d, n->d, n->len);
	q->len = n->len;

	while (!HC_IS_ZERO(q)) {
		q->len = D_div(q->d, q->len, B36_3, q->d, &r);

		s_alloc(s, s->len + 3);

		s->s[s->len++] = HC_BASE36_DIGIT(r % 36);
		s->s[s->len++] = HC_BASE36_DIGIT((r / 36) % 36);
		s->s[s->len++] = HC_BASE36_DIGIT(((r / 36) / 36) % 36);
	}

	while (s->s[s->len - 1] == '0') s->len--;

	b_rev(s->s + s_len0, s->len - s_len0);

	n_free(q);

	return s->len - s_len0;
}

#else
#error not implemented
#endif

int n_cmp_hex(struct n *v, char *hex, int n)
{
	HC_DEF_S(s);
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

	n_as_hex(v, s);
	r = s_diffn(s, hex, n);
	s_free(s);

	return r;
}

int n_cmp_hexz(struct n *v, char *hex)
{
	return n_cmp_hex(v, hex, slen(hex));
}

int n_be1_as_hex(HC_ST_S *s, void *x, int len)
{
	int r;
	HC_DEF_N(n);
	n_load_be1(n, x, len);
	r = n_as_hex(n, s);
	n_free(n);
	return r;
}

int n_be1_as_dec(HC_ST_S *s, void *x, int len)
{
	int r;
	HC_DEF_N(n);
	n_load_be1(n, x, len);
	r = n_as_dec(n, s);
	n_free(n);
	return r;
}

int n_be1_as_base36(HC_ST_S *s, void *x, int len)
{
	int r;
	HC_DEF_N(n);
	n_load_be1(n, x, len);
	r = n_as_base36(n, s);
	n_free(n);
	return r;
}

/**************************************
 */

#define DMULc(a,b,c,p) do {				\
		f _t = ((f)(a))*(b) + (c);	\
		p = DLOW(_t);				\
		c = DHIGH(_t);				\
	} while(0)
#define DMUL(a,b,c1,c0) do {				\
		f _t = ((f)(a))*(b);	\
		c0 = DLOW(_t);				\
		c1 = DHIGH(_t);				\
	} while(0)

#define DDIV(a1,a0,b,q) do {			\
		f _t = DDIGIT((a1),(a0));	\
		q = _t / (b);			\
	} while(0)

#define DDIV2(a1,a0,b1,b0,q) do {		\
		f _t = DDIGIT((a1),(a0));	\
		q = _t / DDIGIT((b1),(b0));	\
	} while(0)

#define DREM(a1,a0,b,r) do {			\
		f _t = DDIGIT((a1),(a0));	\
		r = _t % (b);			\
	} while(0)

/* add a and b with carry in + out */
#define DSUMc(a,b,c,s) do {			\
		h ___cr = (c);		\
		h ___xr = (a)+(___cr);	\
		h ___yr = (b);		\
		___cr = (___xr < ___cr);	\
		___xr = ___yr + ___xr;		\
		___cr += (___xr < ___yr);	\
		s = ___xr;			\
		c = ___cr;			\
	}  while(0)

/* add a and b with carry out */
#define DSUM(a,b,c,s) do {			\
		h ___xr = (a);		\
		h ___yr = (b);		\
		___xr = ___yr + ___xr;		\
		s = ___xr;			\
		c = (___xr < ___yr);		\
	}  while(0)

#define DSUBb(a,b,r,d) do {			\
		h ___cr = (r);		\
		h ___xr = (a);		\
		h ___yr = (b)+___cr;	\
		___cr = (___yr < ___cr);	\
		___yr = ___xr - ___yr;		\
		___cr += (___yr > ___xr);	\
		d = ___yr;			\
		r = ___cr;			\
	} while(0)

#define DSUB(a,b,r,d) do {			\
		h ___xr = (a);		\
		h ___yr = (b);		\
		___yr = ___xr - ___yr;		\
		r = (___yr > ___xr);		\
		d = ___yr;			\
	} while(0)

/* type a constant as a h - to get shifts correct */
#define DCONST(n) ((h)(n))

/*
** compare two number vectors
*/
int I_comp(h *x, int xl, h *y, int yl)
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

#if 0
/*
** Add digits in x and y and store them in r
** assumption: (xl >= yl)
*/
static int I_add(h *x, int xl, h *y, int yl, h *r)
{
	int sz = xl;
	register h yr, xr;
	register h c = 0;

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
#endif

/*
** Add a digits in v1 and store result in vr
*/
static int D_add(h *x, int xl, h c, h *r)
{
	int sz = xl;
	register h xr;

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
static int I_sub(h *x, int xl, h *y, int yl, h *r)
{
	h *r0 = r;
	register h yr, xr;
	register h c = 0;

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
static int D_sub(h *x, int xl, h c, h *r)
{
	h *r0 = r;
	register h yr, xr;

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
static int Z_sub(h *y, int yl, h *r)
{
	h *r0 = r;
	register h yr;
	register h c = 0;

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
int I_mul(h *x, int xl, h *y, int yl, h *r)
{
	h *r0 = r;
	h *rt = r;

	while(xl--) {
		h cp = 0;
		h c = 0;
		int n = yl;
		h *yt = y;
		h d;
		h p;

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

#if 0
/*
** Square digits in x store in r (x & r may point into a common area)
** Assumption: x is destroyed if common area and digits in r are zero
**             to the size of xl+1
*/

static int I_sqr(h *x, int xl, h *r)
{
	h d_next = *x;
	h d;
	h *r0 = r;
	h *s = r;

	if ((r + xl) == x)	/* "Inline" operation */
		*x = 0;
	x++;
	
	while(xl--) {
		h *y = x;
		h y_0 = 0, y_1 = 0, y_2 = 0, y_3 = 0;
		h b0, b1;
		h z0, z1, z2;
		h t;
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
#endif

/*
 * Multiply digits d with digits in x and store in r
 *
 * r[xl+1] = x[xl] * d
 * return r length
 */
int D_mul(h *x, int xl, h d, h *r)
{
	h c = 0;
	int rl = xl;
	h p;

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
static int D_mulsub(h *x, int xl, h d,
			   h *y, int yl, h *r)
{
	h c = 0;
	h b = 0;
	h c0;
	h *r0 = r;
	h s;

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
int D_div(h *x, int xl, h d, h *q, h *r)
{
	h *xp = x + (xl-1);
	h *qp = q + (xl-1);
	int qsz = xl;
	h a1;
	
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
		h q0, a0, b1, b0, b;

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

int I_div(h *x, int xl, h *y, int yl,
		 h *q, h *r, int *rlp)
{
	h *rp;
	h *qp;
	h b1 = y[yl-1];
	h b2 = y[yl-2];
	h a1;
	h a2;
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
		h q0;
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

#if 0
/*
** Remainder of digits in x and a digit d
*/
static h D_rem(h *x, int xl, h d)
{
	h rem = 0;

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
static int I_rem(h *x, int xl, h *y, int yl, h *r)
{
	h *rp;
	h b1 = y[yl-1];
	h b2 = y[yl-2];
	h a1;
	h a2;
	int r_signed = 0;
	int rl;
	
	if (x != r)
		MOVE_DIGITS(r, x, xl);
	rp = r + (xl-yl);
	rl = xl;

	do {
		h q0;
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
static int I_btrail(h *r0, h *r, short sign)
{
	/* convert negative numbers to one complement */
	if (sign) {
		int rl;
		h d;

		/* 1 remove all 0xffff words */
		do {
			r--;
		} while(((d = *r) == D_MASK) && (r != r0));

		/* 2 complement high digit */
		if (d == D_MASK)
			*r = 0;
		else {
			h prev_mask = 0;
			h mask = (DCONST(1)<<(D_EXP-1));

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
static int I_band(h *x, int xl, short xsgn,
			 h *y, int yl, short ysgn, h *r)
{
	h *r0 = r;
	short sign = xsgn && ysgn;

	assert(xl >= yl);

	xl -= yl;

	if (!xsgn) {
		if (!ysgn) {
			while(yl--)
				*r++ = *x++ & *y++;
		}
		else {
			h b;
			h c;

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
			h b;
			h c;

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
			h b1, b2;
			h c1, c2;

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
I_bor(h *x, int xl, short xsgn, h *y,
      int yl, short ysgn, h *r)
{
	h *r0 = r;
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
			h b;
			h c;

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
			h b;
			h c;

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
			h b1, b2;
			h c1, c2;

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
static int I_bxor(h *x, int xl, short xsgn,
			 h *y, int yl, short ysgn, h *r)
{
	h *r0 = r;
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
			h b;
			h c;

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
			h b;
			h c;

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
			h b1, b2;
			h c1, c2;

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
static int I_bnot(h *x, int xl, short xsgn, h *r)
{
	if (xsgn)
		return D_add(x, xl, 1, r);
	else
		return D_sub(x, xl, 1, r);
}

/*
** Arithmetic left shift or right
*/
static int I_lshift(h *x, int xl, int y, 
			   short sign, h *r)
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
		h a1=0;
		h a0=0;

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
			h *r0 = r;
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
				h *z = x;

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
static int I_lg(h *x, int xl)
{
	int sz = xl - 1;
	h d = x[sz];

	sz *= D_EXP;
	while(d != 0) {
		d >>= 1;
		sz++;
	}
	return sz - 1;
}
#endif
