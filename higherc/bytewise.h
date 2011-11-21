#ifndef tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 /* higherc-bytewise-h */
#define tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 /* higherc-bytewise-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

/* code from: git-1.7.1/compat/bswap.h
 */

/*
 * Let's make sure we always have a sane definition for ntohl()/htonl().
 * Some libraries define those as a function call, just to perform byte
 * shifting, bringing significant overhead to what should be a simple
 * operation.
 */

/*
 * Default version that the compiler ought to optimize properly with
 * constant values.
 */
static inline unsigned int hcns(swab32)(unsigned int val)
{
	return (((val & 0xff000000) >> 24) |
		((val & 0x00ff0000) >>  8) |
		((val & 0x0000ff00) <<  8) |
		((val & 0x000000ff) << 24));
}

/*
 */

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

#define HC_ARCH_LE 1

/* __builtin_constant_p: http://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Other-Builtins.html
 */

/* define HC_BSWAP4
 */

#define HC_BSWAP4(x) ({ \
	unsigned int __res; \
	if (__builtin_constant_p(x)) { \
		__res = hcns(swab32)(x);	\
	} else { \
		__asm__("bswap %0" : "=r" (__res) : "0" ((unsigned int)(x))); \
	} \
	__res; })

#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

#define HC_ARCH_LE 1

#include <stdlib.h>

#define HC_BSWAP4(x) _byteswap_ulong(x)

#else

#error unsupported compiler/architecture

#endif

/* define HC_GET_BE4, HC_PUT_BE4
 */

#if defined(HC_BSWAP4) && defined(HC_ARCH_LE)

#define HC_GET_BE4(p)     HC_BSWAP4(*(unsigned int*)(p))
#define HC_PUT_BE4(p, v)  do { *(unsigned int*)(p) = HC_BSWAP4(v); } while (0)

#elif defined(HC_ARCH_LE)

#define HC_GET_BE4(p)	( \
	(*((unsigned char*)(p) + 0) << 24) | \
	(*((unsigned char*)(p) + 1) << 16) | \
	(*((unsigned char*)(p) + 2) <<  8) | \
	(*((unsigned char*)(p) + 3) <<  0) )

#define HC_PUT_BE4(p, v)	do { \
	unsigned int __v = (v); \
	*((unsigned char*)(p) + 0) = __v >> 24; \
	*((unsigned char*)(p) + 1) = __v >> 16; \
	*((unsigned char*)(p) + 2) = __v >>  8; \
	*((unsigned char*)(p) + 3) = __v >>  0; } while (0)

#else

#error unsupported compiler/architecture

#endif

#define HC_BITMASK(c) ((1 << (c)) - ((hcns(u4))1))

/* hex
 */

extern const signed char hcns(hexval_table)[256];

#define HC_HEX_VALUE(d) (hcns(hexval_table)[(unsigned char)(d)])
#define HC_HEX_DIGIT(v) ((v) >= 0 && (v) < 16 ? "0123456789abcdef"[v] : '?')

//#define HC_DEC_VALUE(d)
#define HC_DEC_DIGIT(v) ((v) >= 0 && (v) < 10 ? "0123456789"[v] : '?')

//#define HC_BASE36_VALUE(d)
#define HC_BASE36_DIGIT(v) ((v) >= 0 && (v) < 36 ? "0123456789abcdefghijklmnopqrstuvwxyz"[v] : '?')

/* HC_PUT_HEX(d, n, s): n is the amount of bytes to read from source (s)
 */
#define HC_PUT_HEX(d, n, s)						\
	do {								\
		int __i=0, __n=n;					\
		unsigned char *__d=(unsigned char *)(d);		\
		unsigned char *__s=(unsigned char *)(s);		\
		for (;__i<__n;__i++,__s++) {				\
			*__d++ = HC_HEX_DIGIT(*__s >> 4);		\
			*__d++ = HC_HEX_DIGIT(*__s & 0xf);		\
		}							\
	} while (0);

/* HC_GET_HEX(d, n, s): n is the amount of bytes to write to destiny (d)
 */
#define HC_GET_HEX(d, n, s)						\
	do {								\
		int __i=0, __n=n;					\
		unsigned char *__d=(unsigned char *)(d);		\
		unsigned char *__s=(unsigned char *)(s);		\
		for (;__i<__n;__i++,__d++) {				\
			*__d = HC_HEX_VALUE(*__s++) << 4;		\
			*__d |= HC_HEX_VALUE(*__s++);			\
		}							\
	} while (0);

/* ctype
 */

#define HC_SPACE 0x1 /* space, '\f', '\n', '\r', '\t', '\v' */
#define HC_DIGIT 0x2
#define HC_HEXDIGIT 0x4
#define HC_ALPHA 0x8
#define HC_ALNUM 0x10
#define HC_GLOB 0x20 /* do not match ] */
#define HC_REGEX_META 0x40 /* meta chars: do not match ] and } */
#define HC_REGEX_CHAR 0x80 /* character class: \, ^, -, [, ] */
#define HC_BLANK 0x100 /* space or '\t' */
#define HC_PUNCT 0x200  /* not HC_GLOB nor HC_REGEX_* */

extern hcns(u2) hcns(ctypetbl)[256];

#define HC_CTYPE_FLAGS(x) (hcns(ctypetbl)[(unsigned char)(x)])
#define HC_SANE_ISTEST(x, mask) ((hcns(ctypetbl)[(unsigned char)(x)] & (mask)) != 0)

#define HC_ISASCII(x) (((x) & ~0x7f) == 0)
#define HC_ISSPACE(x) HC_SANE_ISTEST(x, HC_SPACE)
#define HC_ISBLANK(x) HC_SANE_ISTEST(x, HC_BLANK)
#define HC_ISDIGIT(x) HC_SANE_ISTEST(x, HC_DIGIT)
#define HC_ISHEXDIGIT(x) HC_SANE_ISTEST(x, HC_HEXDIGIT)
#define HC_ISALPHA(x) HC_SANE_ISTEST(x, HC_ALPHA)
#define HC_ISALPHA_UPPER(x) ((x & 0x20) == 0)
#define HC_ISALPHA_LOWER(x) ((x & 0x20) != 0)
#define HC_ISALNUM(x) HC_SANE_ISTEST(x, HC_ALNUM)
#define HC_ISPUNCT(x) HC_SANE_ISTEST(x, HC_PUNCT)
#define HC_ISGLOB(x) HC_SANE_ISTEST(x, HC_GLOB)
#define HC_ISREGEX_META(x) HC_SANE_ISTEST(x, HC_REGEX_META)
#define HC_ISREGEX_CHAR(x) HC_SANE_ISTEST(x, HC_REGEX_CHAR)

#define HC_MASK_GRAPH (HC_ALNUM | HC_PUNCT)
#define HC_MASK_PRINT (HC_MASK_GRAPH | HC_SPACE)

#define HC_ISGRAPH(x) HC_SANE_ISTEST(x, HC_MASK_GRAPH)
#define HC_ISPRINT(x) HC_SANE_ISTEST(x, HC_MASK_PRINT)

#define HC_TOLOWER(x) (HC_ISALPHA(x) ? x | 0x20 : x)
#define HC_TOUPPER(x) (HC_ISALPHA(x) ? x & ~0x20 : x)

/*
 * big-endian, 7-bit encoding, msb is set at last digit
 *
 * len0 needs at least 5 digits, use NULL as len0 to get length in enc
 * function
 *
 */
int hcns(enc_u4_be)(void *len0, hcns(u4) v);
hcns(u4) hcns(dec_u4_be)(void *len0, int *lenp);

#endif /* tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 higherc-bytewise-h */
