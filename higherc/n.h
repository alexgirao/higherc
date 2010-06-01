#ifndef tid030be9cfda62t5nche7op21ptiwm8gix4c2xz6l1trg /* higherc-n-h */
#define tid030be9cfda62t5nche7op21ptiwm8gix4c2xz6l1trg /* higherc-n-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

#ifndef tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */
#error higherc-s-h not included
#endif

/* natural numbers, simplest approach, non-cryptography use, motivated
 * to represent sha1 as base36
 *
 * always use native integer types or compiler supported larger types
 */

#define HC_HALF_BITS 16
typedef hcns(u2) hcns(h);    /* half word, at least 16-bit */
typedef hcns(u4) hcns(f);    /* full word, at least 32-bit */

struct hcns(n) {
	hcns(h) *d; /* digits, least significant digit first */
	int len; /* effective words */
	int a; /* allocated words */
};

#define HC_NULL_N {NULL, 0, 0}
#define HC_ST_N struct hcns(n)
#define HC_DEF_N(sym) HC_ST_N sym[1] = {HC_NULL_N}

#define HC_HALF_BYTES (HC_HALF_BITS/8)
#define HC_HALF_MASK ((1<<HC_HALF_BITS)-1)
#define HC_HALF_MAX HC_HALF_MASK

#define HC_FMT_H "%.4x"
#define HC_FMT_F "%.8x"

#define HC_LOW(x)        ((hcns(h))(x))
#define HC_HIGH(x)       ((hcns(h))(((hcns(f))(x)) >> HC_HALF_BITS))
#define HC_TO_HIGH(x)    (((hcns(f))(x)) << HC_HALF_BITS)

#define HC_H(v) ((hcns(h))(v))
#define HC_F(hi,lo) ((hcns(f))(HC_TO_HIGH(hi)+(lo)))

#define HC_ZERO_DIGITS(v, l) do {		\
		int __l = l;		\
		hcns(h) *__v  = v;		\
		while(__l--) *__v++ = 0;	\
	} while(0)

#define HC_MOVE_DIGITS(dst, src, l) do {			\
		int __l = l;				\
		hcns(h) *__dst;					\
		hcns(h) *__src;					\
		if (dst < src) {				\
			__dst = dst;				\
			__src = src;				\
			while(__l--) *__dst++ = *__src++;	\
		} else if (dst > src) {				\
			__dst = (dst)+((l)-1);			\
			__src = (src)+((l)-1);			\
			while(__l--) *__dst-- = *__src--;	\
		}						\
	} while(0)

#define HC_IS_ZERO(n) ((n) == NULL || (n)->len == 0 || ((n)->len == 1 && (n)->d[0] == HC_H(0)))

void hcns(n_alloc)(struct hcns(n) *x, int n);
void hcns(n_free)(struct hcns(n) *x);

void hcns(n_copyn)(struct hcns(n) *x, const hcns(h) *d, int n);
void hcns(n_copy)(struct hcns(n) *to, const struct hcns(n) *from);

void hcns(n_set_u4)(struct hcns(n) *n, hcns(u4) v);

int hcns(n_as_hex)(struct hcns(n) *n, struct hcns(s) *s);
int hcns(n_as_dec)(struct hcns(n) *n, struct hcns(s) *s);
int hcns(n_as_base36)(struct hcns(n) *n, struct hcns(s) *s);

void hcns(n_load_be1)(struct hcns(n) *r, void *x, int len);  /* load big-endian bytes */
void hcns(n_load_hex)(struct hcns(n) *r, char *hex, int n);  /* load hex string */
void hcns(n_load_hexz)(struct hcns(n) *r, char *hex); /* likewise */

int hcns(n_cmp_hex)(struct hcns(n) *v, char *hex, int n);
int hcns(n_cmp_hexz)(struct hcns(n) *v, char *hex);

int hcns(n_be1_as_hex)(struct hcns(s) *s, void *x, int len);
int hcns(n_be1_as_dec)(struct hcns(s) *s, void *x, int len);
int hcns(n_be1_as_base36)(struct hcns(s) *s, void *x, int len);

int D_mul(hcns(h) *x, int xl, hcns(h) d, hcns(h) *r);
int D_div(hcns(h) *x, int xl, hcns(h) d, hcns(h) *q, hcns(h) *r);

int I_mul(hcns(h) *x, int xl, hcns(h) *y, int yl, hcns(h) *r);
int I_comp(hcns(h) *x, int xl, hcns(h) *y, int yl);
int I_div(hcns(h) *x, int xl, hcns(h) *y, int yl, hcns(h) *q, hcns(h) *r, int *rlp);

#endif
