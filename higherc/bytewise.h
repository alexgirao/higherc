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

#endif /* tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 higherc-bytewise-h */
