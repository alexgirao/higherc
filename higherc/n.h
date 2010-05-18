#ifndef tid030be9cfda62t5nche7op21ptiwm8gix4c2xz6l1trg /* higherc-n-h */
#define tid030be9cfda62t5nche7op21ptiwm8gix4c2xz6l1trg /* higherc-n-h */

/* natural numbers routines
 */

typedef unsigned short hcns(u2);
typedef unsigned int hcns(u4);
//typedef unsigned long hcns(u8);

typedef hcns(u2) hcns(h);    /* half-digit, at least 16-bit */
typedef hcns(u4) hcns(f);    /* full-digit, at least 32-bit */

#define HC_HALF_BITS 16
#define HC_HALF_LOMASK 0xffff
#define HC_HALF_HIMASK 0xffff0000

#define HC_FMT_H "%.4x"
#define HC_FMT_F "%.8x"

struct hcns(n) {
	hcns(f) *d; /* digits */
	int len; /* effective words */
	int a; /* allocated words */
};

/* literal digit
 */
#define HC_H(v) ((hcns(h))v)
#define HC_F(v) ((hcns(f))v)

#define HC_LOW(a)      ((a)&HC_HALF_LOMASK)
#define HC_HIGH(a)     (((a)&HC_HALF_HIMASK)>>HC_HALF_BITS)
#define HC_TO_HIGH(a)  ((hcns(f))((a)<<HC_HALF_BITS))

#endif
