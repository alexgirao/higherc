#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/list.h"
#include "higherc/byte.h"
#include "higherc/bytewise.h"
#include "higherc/s.h"
#include "higherc/str.h"
#include "higherc/rand.h"
#include "higherc/fatal.h"
#include "higherc/n.h"

#define DEBUG_N(n) do {							\
		printf(__FILE__ ":%i: ", __LINE__);			\
		print_n(#n " [", n, "]");				\
		printf(" (d: %p, len: %i, a: %i)\n", (n)->d, (n)->len, (n)->a); \
		fflush(stdout);						\
	} while (0)

#define ASSERT assert
#define D_EXP HC_HALF_BITS
#define D_MASK HC_HALF_MASK

#define ZERO_DIGITS(v, sz) do {			\
	hcns(uint) _t_sz = sz;			\
	hcns(h)* _t_v  = v;			\
	while(_t_sz--) *_t_v++ = 0;		\
    } while(0)

#define MOVE_DIGITS(dst, src, sz) do {				\
	hcns(uint) _t_sz = sz;					\
	hcns(h)* _t_dst;					\
	hcns(h)* _t_src;					\
	if (dst < src) {					\
	    _t_dst = dst;					\
	    _t_src = src;					\
	    while(_t_sz--) *_t_dst++ = *_t_src++;		\
	}							\
	else if (dst > src) {					\
	    _t_dst = (dst)+((sz)-1);				\
	    _t_src = (src)+((sz)-1);				\
	    while(_t_sz--) *_t_dst-- = *_t_src--;		\
	}							\
    } while(0)

/* add a and b with carry in + out */
#define DSUMc(a,b,c,s) do {						\
	hcns(h) ___cr = (c);						\
	hcns(h) ___xr = (a)+(___cr);					\
	hcns(h) ___yr = (b);						\
	___cr = (___xr < ___cr);					\
	___xr = ___yr + ___xr;						\
	___cr += (___xr < ___yr);					\
	s = ___xr;							\
	c = ___cr;							\
    }  while(0)

/* add a and b with carry out */
#define DSUM(a,b,c,s) do {					\
	hcns(h) ___xr = (a);					\
	hcns(h) ___yr = (b);					\
	___xr = ___yr + ___xr;					\
	s = ___xr;						\
	c = (___xr < ___yr);					\
    }  while(0)

#define DSUBb(a,b,r,d) do {						\
	hcns(h) ___cr = (r);						\
	hcns(h) ___xr = (a);						\
	hcns(h) ___yr = (b)+___cr;					\
	___cr = (___yr < ___cr);					\
	___yr = ___xr - ___yr;						\
	___cr += (___yr > ___xr);					\
	d = ___yr;							\
	r = ___cr;							\
    } while(0)

#define DSUB(a,b,r,d) do {			\
	hcns(h) ___xr = (a);			\
	hcns(h) ___yr = (b);			\
	___yr = ___xr - ___yr;			\
	r = (___yr > ___xr);			\
	d = ___yr;				\
    } while(0)

/* type a constant as a hcns(h) - to get shifts correct */
#define DCONST(n) ((hcns(h))(n))

/*
** compare two number vectors
*/
static int I_comp(hcns(h)* x, hcns(uint) xl, hcns(h)* y, hcns(uint) yl)
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
static hcns(uint) I_add(hcns(h)* x, hcns(uint) xl, hcns(h)* y, hcns(uint) yl, hcns(h)* r)
{
    hcns(uint) sz = xl;
    register hcns(h) yr, xr;
    register hcns(h) c = 0;

    ASSERT(xl >= yl);

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
static hcns(uint) D_add(hcns(h)* x, hcns(uint) xl, hcns(h) c, hcns(h)* r)
{
    hcns(uint) sz = xl;
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
static hcns(uint) I_sub(hcns(h)* x, hcns(uint) xl, hcns(h)* y, hcns(uint) yl, hcns(h)* r)
{
    hcns(h)* r0 = r;
    register hcns(h) yr, xr;
    register hcns(h) c = 0;

    ASSERT(I_comp(x, xl, y, yl) >= 0);

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
static hcns(uint) D_sub(hcns(h)* x, hcns(uint) xl, hcns(h) c, hcns(h)* r)
{
    hcns(h)* r0 = r;
    register hcns(h) yr, xr;

    ASSERT(I_comp(x, xl, x, 1) >= 0);

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
static hcns(uint) Z_sub(hcns(h)* y, hcns(uint) yl, hcns(h)* r)
{
    hcns(h)* r0 = r;
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
static hcns(uint) I_mul(hcns(h)* x, hcns(uint) xl, hcns(h)* y, hcns(uint) yl, hcns(h)* r)
{
    hcns(h) *r0 = r;
    hcns(h) *rt = r;

    while(xl--) {
	hcns(h) cp = 0;
	hcns(h) c = 0;
	hcns(uint) n = yl;
	hcns(h)* yt = y;
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

static hcns(uint) I_sqr(hcns(h)* x, hcns(uint) xl, hcns(h)* r)
{
    hcns(h) d_next = *x;
    hcns(h) d;
    hcns(h)* r0 = r;
    hcns(h)* s = r;

    if ((r + xl) == x)	/* "Inline" operation */
	*x = 0;
    x++;
	
    while(xl--) {
	hcns(h)* y = x;
	hcns(h) y_0 = 0, y_1 = 0, y_2 = 0, y_3 = 0;
	hcns(h) b0, b1;
	hcns(h) z0, z1, z2;
	hcns(h) t;
	hcns(uint) y_l = xl;
		
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
	    ASSERT((y_1+y_2+y_3) == 0);
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
static hcns(uint) D_mul(hcns(h)* x, hcns(uint) xl, hcns(h) d, hcns(h)* r)
{
    hcns(h) c = 0;
    hcns(uint) rl = xl;
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
static hcns(uint) D_mulsub(hcns(h)* x, hcns(uint) xl, hcns(h) d,
			hcns(h)* y, hcns(uint) yl, hcns(h)* r)
{
    hcns(h) c = 0;
    hcns(h) b = 0;
    hcns(h) c0;
    hcns(h)* r0 = r;
    hcns(h) s;

    ASSERT(xl == yl || xl == yl+1);

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
static hcns(uint) D_div(hcns(h)* x, hcns(uint) xl, hcns(h) d, hcns(h)* q, hcns(h)* r)
{
    hcns(h)* xp = x + (xl-1);
    hcns(h)* qp = q + (xl-1);
    hcns(uint) qsz = xl;
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

static hcns(uint) I_div(hcns(h)* x, hcns(uint) xl, hcns(h)* y, hcns(uint) yl,
		     hcns(h)* q, hcns(h)* r, hcns(uint)* rlp)
{
    hcns(h)* rp;
    hcns(h)* qp;
    hcns(h) b1 = y[yl-1];
    hcns(h) b2 = y[yl-2];
    hcns(h) a1;
    hcns(h) a2;
    int r_signed = 0;
    hcns(uint) ql;
    hcns(uint) rl;

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
	hcns(uint) nsz = yl;
	hcns(uint) nnsz;

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
static hcns(h) D_rem(hcns(h)* x, hcns(uint) xl, hcns(h) d)
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
static hcns(uint) I_rem(hcns(h)* x, hcns(uint) xl, hcns(h)* y, hcns(uint) yl, hcns(h)* r)
{
    hcns(h)* rp;
    hcns(h) b1 = y[yl-1];
    hcns(h) b2 = y[yl-2];
    hcns(h) a1;
    hcns(h) a2;
    int r_signed = 0;
    hcns(uint) rl;
	
    if (x != r)
	MOVE_DIGITS(r, x, xl);
    rp = r + (xl-yl);
    rl = xl;

    do {
	hcns(h) q0;
	hcns(uint) nsz = yl;
	hcns(uint) nnsz;
		
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
static hcns(uint) I_btrail(hcns(h)* r0, hcns(h)* r, short sign)
{
    /* convert negative numbers to one complement */
    if (sign) {
	hcns(uint) rl;
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
	    hcns(h) mask = (DCONST(1) << (D_EXP-1));

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
static hcns(uint) I_band(hcns(h)* x, hcns(uint) xl, short xsgn,
		      hcns(h)* y, hcns(uint) yl, short ysgn, hcns(h)* r)
{
    hcns(h)* r0 = r;
    short sign = xsgn && ysgn;

    ASSERT(xl >= yl);

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
static hcns(uint)
I_bor(hcns(h)* x, hcns(uint) xl, short xsgn, hcns(h)* y,
      hcns(uint) yl, short ysgn, hcns(h)* r)
{
    hcns(h)* r0 = r;
    short sign = xsgn || ysgn;

    ASSERT(xl >= yl);

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
static hcns(uint) I_bxor(hcns(h)* x, hcns(uint) xl, short xsgn,
		      hcns(h)* y, hcns(uint) yl, short ysgn, hcns(h)* r)
{
    hcns(h)* r0 = r;
    short sign = xsgn != ysgn;

    ASSERT(xl >= yl);

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
static hcns(uint) I_bnot(hcns(h)* x, hcns(uint) xl, short xsgn, hcns(h)* r)
{
    if (xsgn)
	return D_add(x, xl, 1, r);
    else
	return D_sub(x, xl, 1, r);
}

/*
** Arithmetic left shift or right
*/
static hcns(uint) I_lshift(hcns(h)* x, hcns(uint) xl, int y, 
			short sign, hcns(h)* r)
{
    if (y == 0) {
	MOVE_DIGITS(r, x, xl);
	return xl;
    }
    else if (xl == 1 && *x == 0) {
	*r = 0;
	return 1;
    }
    else {
	long ay = (y < 0) ? -y : y;
	int bw = ay / D_EXP;
	int sw = ay % D_EXP;
	hcns(uint) rl;
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
	}
	else {			/* shift right */
	    hcns(h)* r0 = r;
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
		hcns(h)* z = x;

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
	    }
	    else {
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
static int I_lg(hcns(h)* x, hcns(uint) xl)
{
    hcns(uint) sz = xl - 1;
    hcns(h) d = x[sz];

    sz *= D_EXP;
    while(d != 0) {
	d >>= 1;
	sz++;
    }
    return sz - 1;
}

/*
 */

static void print_n(char *prefix, struct hcns(n) *n, char *suffix)
{
	struct hcns(s) s = HC_NULL_S;

	//assert(n->d != NULL);
	//assert(n->len > 0);

	hcns(s_catz)(&s, prefix);
	hcns(n_as_hex)(n, &s);
	hcns(s_catz)(&s, suffix);

	HC_SAFE_CSTR(&s);

	printf("%s", s.s);

	hcns(s_free)(&s);
}

static int n_cmp_hex(struct hcns(n) *n, char *hex)
{
	struct hcns(s) s = HC_NULL_S;
	int r;

	hcns(n_as_hex)(n, &s);
	HC_SAFE_CSTR(&s);

	r = hcns(s_sdiff)(&s, hex);

	hcns(s_free)(&s);

	return r;
}

static void test_MUL()
{
	hcns(h) a[2];
	hcns(h) b;
	hcns(h) c;

	b = HC_HALF_OFFENSE;
	c = HC_HALF_OFFENSE;

	DMUL(b, c, a[1], a[0]);

	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xc1b0 && a[0] == 0x80e9);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul
	 */

	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = HC_HALF_OFFENSE;
	a[0] = HC_HALF_OFFENSE;

	DMUL(b, c, a[1], a[0]);

	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0001);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul_c without carry
	 */
		
	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = 0;  /* carry goes here */
	a[0] = HC_HALF_OFFENSE;
		
	DMULc(b, c, a[1], a[0]);
	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0001);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul_c with carry
	 */
		
	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = 1;  /* carry goes here */
	a[0] = HC_HALF_OFFENSE;
		
	DMULc(b, c, a[1], a[0]);
	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0002);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul_c with carry
	 */
		
	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = 2;  /* carry goes here */
	a[0] = HC_HALF_OFFENSE;
		
	DMULc(b, c, a[1], a[0]);
	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0003);
	} else {
		HC_FATAL("not implemented");
	}
}

static void test_hex_in_out()
{
	char hexstr[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	assert(sizeof(hexstr) == 40);

	{
		struct hcns(n) a = HC_NULL_N;
		struct hcns(s) s = HC_NULL_S;
		unsigned char valstr[sizeof(hexstr) / 2];
	
		HC_GET_HEX(valstr, sizeof(valstr), hexstr);
		assert(valstr[0] == 0xda && valstr[sizeof(valstr) - 1] == 0x09);

		hcns(n_load_be1)(&a, valstr, sizeof(valstr));
		if (HC_HALF_BITS == 16) {
			assert(a.d[a.len-1] == 0xda39 && a.d[a.len-2] == 0xa3ee && a.d[1] == 0xafd8 && a.d[0] == 0x0709);
		} else {
			HC_FATAL("not implemented");
		}

		s.len = 0;
		hcns(n_as_hex)(&a, &s);
		HC_SAFE_CSTR(&s);

		assert(hcns(sdiffn)(s.s, hexstr, sizeof(hexstr)) == 0);

		hcns(s_free)(&s);
		hcns(n_free)(&a);
	}

	{
		struct hcns(n) a = HC_NULL_N;
		struct hcns(s) s = HC_NULL_S;

		hcns(n_load_hex)(&a, hexstr, sizeof(hexstr));
		if (HC_HALF_BITS == 16) {
			assert(a.d[a.len-1] == 0xda39 && a.d[a.len-2] == 0xa3ee && a.d[1] == 0xafd8 && a.d[0] == 0x0709);
		} else {
			HC_FATAL("not implemented");
		}

		s.len = 0;
		hcns(n_as_hex)(&a, &s);
		HC_SAFE_CSTR(&s);

		assert(hcns(sdiffn)(s.s, hexstr, sizeof(hexstr)) == 0);

		hcns(s_free)(&s);
		hcns(n_free)(&a);
	}

	{
		struct hcns(n) a[1] = {HC_NULL_N};

		assert(sizeof(a) == sizeof(struct hcns(n)));

		hcns(n_set_u4)(a, 0x00000001U);
		assert(n_cmp_hex(a, "1") == 0);

		hcns(n_set_u4)(a, 0x00000012U);
		assert(n_cmp_hex(a, "12") == 0);

		hcns(n_set_u4)(a, 0x00000123U);
		assert(n_cmp_hex(a, "123") == 0);

		hcns(n_set_u4)(a, 0x00001234U);
		assert(n_cmp_hex(a, "1234") == 0);

		hcns(n_set_u4)(a, 0x00012345U);
		assert(n_cmp_hex(a, "12345") == 0);

		hcns(n_set_u4)(a, 0x00123456U);
		assert(n_cmp_hex(a, "123456") == 0);

		hcns(n_set_u4)(a, 0x01234567U);
		assert(n_cmp_hex(a, "1234567") == 0);

		hcns(n_set_u4)(a, 0x12345678U);
		assert(n_cmp_hex(a, "12345678") == 0);

		hcns(n_set_u4)(a, 0x23456789U);
		assert(n_cmp_hex(a, "23456789") == 0);

		hcns(n_set_u4)(a, 0x34567890U);
		assert(n_cmp_hex(a, "34567890") == 0);

		hcns(n_set_u4)(a, 0x45678901U);
		assert(n_cmp_hex(a, "45678901") == 0);

		hcns(n_set_u4)(a, 0x56789012U);
		assert(n_cmp_hex(a, "56789012") == 0);

		hcns(n_set_u4)(a, 0xdeadbeefU);
		assert(n_cmp_hex(a, "deadbeef") == 0);

		hcns(n_free)(a);
	}

	{
		struct hcns(n) a[1] = {HC_NULL_N};

		hcns(n_load_hexz)(a, "1");
		hcns(n_load_hexz)(a, "12");
		hcns(n_load_hexz)(a, "123");
		hcns(n_load_hexz)(a, "1234");
		hcns(n_load_hexz)(a, "12345");
		hcns(n_load_hexz)(a, "00012345");
		hcns(n_load_hexz)(a, "123456");
		hcns(n_load_hexz)(a, "1234567");
	}
}

static void test_mul1()
{
	char a_hex[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	struct hcns(n) a = HC_NULL_N;
	struct hcns(n) r = HC_NULL_N;

	hcns(n_load_hex)(&a, a_hex, sizeof(a_hex));

	hcns(n_alloc)(&r, a.len + 1);

	r.len = D_mul(a.d, a.len, HC_H(0), r.d);
	assert(n_cmp_hex(&r, "0") == 0);

	if (HC_HALF_BITS == 16) {
		r.len = D_mul(a.d, a.len, HC_H(1), r.d);
		assert(n_cmp_hex(&r, "da39a3ee5e6b4b0d3255bfef95601890afd80709") == 0);

		r.len = D_mul(a.d, a.len, HC_HALF_OFFENSE, r.d);
		assert(n_cmp_hex(&r, "bdd1751c7df6d997299160647871473e12442b168f15") == 0);

		r.len = D_mul(a.d, a.len, HC_HALF_MAX, r.d);
		assert(n_cmp_hex(&r, "da38c9b4ba7ceca1e7488d99d570833097475730f8f7") == 0);
	} else {
		HC_FATAL("not implemented");
	}

	hcns(n_free)(&a);
	hcns(n_free)(&r);
}

static void test_muln()
{
	char *tests0[] = {
		#include "sandbox/bignum.escript--c_verify"
		NULL
	};
	char **tests = tests0;

	void verify(char *a_hex, char *b_hex, char *expected)
	{
		struct hcns(n) a[1] = {HC_NULL_N};
		struct hcns(n) b[1] = {HC_NULL_N};
		struct hcns(n) r[1] = {HC_NULL_N};
		int max_digits;

		int a_len = hcns(slen)(a_hex);
		int b_len = hcns(slen)(b_hex);

		hcns(n_load_hex)(a, a_hex, a_len);
		hcns(n_load_hex)(b, b_hex, b_len);

		assert(n_cmp_hex(a, a_hex) == 0);
		assert(n_cmp_hex(b, b_hex) == 0);

		hcns(n_alloc)(r, a->len + b->len);

		max_digits = a->len > b->len ? a->len : b->len;

		/* r = a * b
		 */

		ZERO_DIGITS(r->d, max_digits);
		r->len = I_mul(a->d, a->len, b->d, b->len, r->d);
		assert(n_cmp_hex(r, expected) == 0);

		/* r = b * a
		 */

		ZERO_DIGITS(r->d, max_digits);
		r->len = I_mul(b->d, b->len, a->d, a->len, r->d);
		assert(n_cmp_hex(r, expected) == 0);

		hcns(n_free)(r);
		hcns(n_free)(a);
		hcns(n_free)(b);
	}

	while (*tests) {
		char *a = *tests++;
		char *b = *tests++;
		char *e = *tests++;

		assert(a);
		assert(b);
		assert(e);

		verify(a, b, e);
		verify(b, a, e);   /* double inversion */
	}
}

int main(int argc, char **argv)
{
	if (0) {
		struct hcns(n) _unused = HC_NULL_N;
		DEBUG_N(&_unused);
	}

	assert(sizeof(hcns(u1)) == 1);
	assert(sizeof(hcns(u2)) == 2);
	assert(sizeof(hcns(u4)) == 4);
//	assert(sizeof(hcns(u8)) == 8);   // future

	if(1){
	test_MUL();  // DMUL
	}
	test_hex_in_out();
	if (1) {
	test_mul1(); // D_mul
	test_muln();
	}

	return 0;
}
