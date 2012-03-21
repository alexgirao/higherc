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
typedef uint16_t h;    /* half word, at least 16-bit */
typedef uint32_t f;    /* full word, at least 32-bit */

struct n {
	h *d; /* digits, least significant digit first */
	int len; /* effective words */
	int a; /* allocated words */
};

#define HC_NULL_N {NULL, 0, 0}
#define HC_ST_N struct n
#define HC_DEF_N(sym) HC_ST_N sym[1] = {HC_NULL_N}

#define HC_HALF_BYTES (HC_HALF_BITS/8)
#define HC_HALF_MASK ((1<<HC_HALF_BITS)-1)
#define HC_HALF_MAX HC_HALF_MASK

#define HC_FMT_H "%.4x"
#define HC_FMT_F "%.8x"

#define HC_LOW(x)        ((h)(x))
#define HC_HIGH(x)       ((h)(((f)(x)) >> HC_HALF_BITS))
#define HC_TO_HIGH(x)    (((f)(x)) << HC_HALF_BITS)

#define HC_H(v) ((h)(v))
#define HC_F(hi,lo) ((f)(HC_TO_HIGH(hi)+(lo)))

#define HC_ZERO_DIGITS(v, l) do {		\
		int __l = l;		\
		h *__v  = v;		\
		while(__l--) *__v++ = 0;	\
	} while(0)

#define HC_MOVE_DIGITS(dst, src, l) do {			\
		int __l = l;				\
		h *__dst;					\
		h *__src;					\
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

void n_alloc(struct n *x, int n);
void n_free(struct n *x);

void n_copyn(struct n *x, const h *d, int n);
void n_copy(struct n *to, const struct n *from);

void n_set_u4(struct n *n, uint32_t v);

int n_as_hex(struct n *n, struct s *s);
int n_as_dec(struct n *n, struct s *s);
int n_as_base36(struct n *n, struct s *s);

void n_load_be1(struct n *r, void *x, int len);  /* load big-endian bytes */
void n_load_hex(struct n *r, char *hex, int n);  /* load hex string */
void n_load_hexz(struct n *r, char *hex); /* likewise */

int n_cmp_hex(struct n *v, char *hex, int n);
int n_cmp_hexz(struct n *v, char *hex);

int n_be1_as_hex(struct s *s, void *x, int len);
int n_be1_as_dec(struct s *s, void *x, int len);
int n_be1_as_base36(struct s *s, void *x, int len);

int D_mul(h *x, int xl, h d, h *r);
int D_div(h *x, int xl, h d, h *q, h *r);

int I_mul(h *x, int xl, h *y, int yl, h *r);
int I_comp(h *x, int xl, h *y, int yl);
int I_div(h *x, int xl, h *y, int yl, h *q, h *r, int *rlp);

#endif
