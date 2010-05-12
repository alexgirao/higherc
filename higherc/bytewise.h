#ifndef tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 /* higherc-bytewise-h */
#define tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 /* higherc-bytewise-h */

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

#define HC_GET_BE4(p)     HC_BSWAP4(*(unsigned int *)(p))
#define HC_PUT_BE4(p, v)  do { *(unsigned int *)(p) = HC_BSWAP4(v); } while (0)

#elif defined(HC_ARCH_LE)

#define HC_GET_BE4(p)	( \
	(*((unsigned char *)(p) + 0) << 24) | \
	(*((unsigned char *)(p) + 1) << 16) | \
	(*((unsigned char *)(p) + 2) <<  8) | \
	(*((unsigned char *)(p) + 3) <<  0) )

#define HC_PUT_BE4(p, v)	do { \
	unsigned int __v = (v); \
	*((unsigned char *)(p) + 0) = __v >> 24; \
	*((unsigned char *)(p) + 1) = __v >> 16; \
	*((unsigned char *)(p) + 2) = __v >>  8; \
	*((unsigned char *)(p) + 3) = __v >>  0; } while (0)

#else

#error unsupported compiler/architecture

#endif

/* hex
 */

extern const signed char hcns(hexval_table)[256];
/*static inline unsigned int hcns(hexval)(unsigned char c)
{
	return hcns(hexval_table)[c];
	}*/

#define HC_HEX_VALUE(d) (hcns(hexval_table)[(unsigned char)(d)])
#define HC_HEX_DIGIT(v) ("0123456789abcdef"[(v)&0xf])

/* HC_PUT_HEX(d, n, s): n is the amount of bytes to read from source (s)
 */
#define HC_PUT_HEX(d, n, s)						\
	do {								\
		int __i=0, __n=n;					\
		unsigned char *__d=(unsigned char *)(d);		\
		unsigned char *__s=(unsigned char *)(s);		\
		for (;__i<__n;__i++,__s++) {				\
			*__d++ = HC_HEX_DIGIT(*__s >> 4);		\
			*__d++ = HC_HEX_DIGIT(*__s);			\
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

#define GIT_SPACE 0x01
#define GIT_DIGIT 0x02
#define GIT_ALPHA 0x04
#define GIT_GLOB_SPECIAL 0x08 /* do not match ), ] and } */
#define GIT_REGEX_SPECIAL 0x10
#define GIT_BLANK 0x20
#define GIT_PUNCT 0x40  /* not GIT_GLOB_SPECIAL nor GIT_REGEX_SPECIAL */
//#define GIT_ 0x80

extern unsigned char sane_ctype[256];

#define getflags(x) sane_ctype[(unsigned char)(x)]
#define sane_istest(x, mask) ((sane_ctype[(unsigned char)(x)] & (mask)) != 0)

#define MASK_ALNUM (GIT_ALPHA | GIT_DIGIT)
#define MASK_GRAPH (MASK_ALNUM | GIT_GLOB_SPECIAL | GIT_REGEX_SPECIAL | GIT_PUNCT)
#define MASK_PRINT (MASK_GRAPH | GIT_SPACE)

#define isascii(x) (((x) & ~0x7f) == 0)
#define isspace(x) sane_istest(x, GIT_SPACE)
#define isblank(x) sane_istest(x, GIT_BLANK)
#define isdigit(x) sane_istest(x, GIT_DIGIT)
#define isalpha(x) sane_istest(x, GIT_ALPHA)
#define isalnum(x) sane_istest(x, MASK_ALNUM)
#define isgraph(x) sane_istest(x, MASK_GRAPH)
#define isprint(x) sane_istest(x, MASK_PRINT)
#define ispunct(x) sane_istest(x, MASK_GRAPH & ~MASK_ALNUM)
#define is_glob_special(x) sane_istest(x, GIT_GLOB_SPECIAL)
#define is_regex_special(x) sane_istest(x, GIT_GLOB_SPECIAL | GIT_REGEX_SPECIAL)

#define tolower(x) (isalpha(x) ? x | 0x20 : x)
#define toupper(x) (isalpha(x) ? x & ~0x20 : x)

#endif /* tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 higherc-bytewise-h */
