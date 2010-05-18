#ifndef tid030be9cfda62t5nche7op21ptiwm8gix4c2xz6l1trg /* higherc-n-h */
#define tid030be9cfda62t5nche7op21ptiwm8gix4c2xz6l1trg /* higherc-n-h */

/* natural numbers, simplest approach, non-cryptography use, motivated
 * to represent sha1 as base36
 */

typedef unsigned short hcns(u2);
typedef unsigned int hcns(u4);
//typedef unsigned long hcns(u8);

typedef hcns(u2) hcns(h);    /* half-digit, at least 16-bit */
typedef hcns(u4) hcns(f);    /* full-digit, at least 32-bit */

#define HC_HALF_BITS 16
#define HC_HALF_MASK 0xffff
#define HC_HALF_OFFENSE 0xdeadU  /* ``offensive programming'' */ 
#define HC_OFFENSE 0xdeaddeadU   /* ``offensive programming'' */ 

#define HC_FMT_H "%.4x"
#define HC_FMT_F "%.8x"

struct hcns(n) {
	hcns(h) *d; /* digits */
	int len; /* effective words */
	int a; /* allocated words */
};

/* literal digit
 */
#define HC_H(v) ((hcns(h))v)
#define HC_F(v) ((hcns(f))v)

#define HC_LOW(a)      ((a) & HC_HALF_MASK)
#define HC_HIGH(a)     (((a) >> HC_HALF_BITS) & HC_HALF_MASK)
#define HC_TO_HIGH(a)  ((hcns(f))((hcns(h))(a) << HC_HALF_BITS))

/* macros that operate directly on digits
 */

#define HC_ZERO(v, sz) do {			\
		int _t_sz = sz;			\
		hcns(h) *_t_v  = v;		\
		while(_t_sz--) *_t_v++ = 0;	\
	} while(0)

#define HC_MOVE(dst, src, sz) do {				\
		int _t_sz = sz;					\
		hcns(h) *_t_dst;				\
		hcns(h) *_t_src;				\
		if (dst < src) {				\
			_t_dst = dst;				\
			_t_src = src;				\
			while(_t_sz--) *_t_dst++ = *_t_src++;	\
		} else if (dst > src) {				\
			_t_dst = (dst)+((sz)-1);		\
			_t_src = (src)+((sz)-1);		\
			while(_t_sz--) *_t_dst-- = *_t_src--;	\
		}						\
	} while(0)

/* add a and b with carry (c) in + out
 */
#define HC_ADD_C(a,b,c,s) do {						\
		hcns(h) ___cr = (c);					\
		hcns(h) ___xr = (a)+(___cr);				\
		hcns(h) ___yr = (b);					\
		___cr = (___xr < ___cr);				\
		___xr = ___yr + ___xr;					\
		___cr += (___xr < ___yr);				\
		s = ___xr;						\
		c = ___cr;						\
	}  while(0)

/* add a and b with carry (c) out
 */
#define HC_ADD(a,b,c,s) do {					\
		hcns(h) ___xr = (a);				\
		hcns(h) ___yr = (b);				\
		___xr = ___yr + ___xr;				\
		s = ___xr;					\
		c = (___xr < ___yr);				\
	}  while(0)

/* subtract a and b with borrow (d) in + out
 */
#define HC_SUB_B(a,b,r,d) do {						\
		hcns(h) ___cr = (r);					\
		hcns(h) ___xr = (a);					\
		hcns(h) ___yr = (b)+___cr;				\
		___cr = (___yr < ___cr);				\
		___yr = ___xr - ___yr;					\
		___cr += (___yr > ___xr);				\
		d = ___yr;						\
		r = ___cr;						\
	} while(0)

/* subtract a and b with borrow (d) out
 */
#define HC_SUB(a,b,r,d) do {			\
		hcns(h) ___xr = (a);		\
		hcns(h) ___yr = (b);		\
		___yr = ___xr - ___yr;		\
		r = (___yr > ___xr);		\
		d = ___yr;			\
	} while(0)

#define D_EXP  HC_HALF_BITS
#define DLOW(x)        ((hcns(h))(x))
#define DHIGH(x)       ((hcns(h))(((hcns(f))(x)) >> D_EXP))

/* ErtsDigit => ErtsDoubleDigit */
#define DLOW2HIGH(x)   (((hcns(f))(x)) << D_EXP)
#define DDIGIT(a1,a0)  (DLOW2HIGH(a1) + (a0))

#define DMULc(a,b,c,p) do {			       \
        hcns(f) _t = ((hcns(f))(a))*(b) + (c);	\
	p = DLOW(_t);						\
	c = DHIGH(_t);						\
    } while(0)
#define DMUL(a,b,c1,c0) do { \
	hcns(f) _t = ((hcns(f))(a))*(b);	\
	c0 = DLOW(_t);					\
	c1 = DHIGH(_t);					\
    } while(0)

#define DDIV(a1,a0,b,q) do {						\
	hcns(f) _t = DDIGIT((a1),(a0));				\
	q = _t / (b);							\
    } while(0)

#define DDIV2(a1,a0,b1,b0,q) do {					\
	hcns(f) _t = DDIGIT((a1),(a0));				\
	q = _t / DDIGIT((b1),(b0));					\
    } while(0)

#define DREM(a1,a0,b,r) do { \
	hcns(f) _t = DDIGIT((a1),(a0));		\
	r = _t % (b);					\
    } while(0)

#endif
