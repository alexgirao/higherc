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
static inline unsigned int hcns(_swab32)(unsigned int val)
{
	return (((val & 0xff000000) >> 24) |
		((val & 0x00ff0000) >>  8) |
		((val & 0x0000ff00) <<  8) |
		((val & 0x000000ff) << 24));
}

/*
 */

#if defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))

/* __builtin_constant_p: http://gcc.gnu.org/onlinedocs/gcc-4.3.2/gcc/Other-Builtins.html
 */

/* define HC_BSWAP4
 */

#define HC_BSWAP4(x) ({ \
	unsigned int __res; \
	if (__builtin_constant_p(x)) { \
		__res = hcns(_swab32)(x);	\
	} else { \
		__asm__("bswap %0" : "=r" (__res) : "0" ((unsigned int)(x))); \
	} \
	__res; })

#elif defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))

#include <stdlib.h>

#define HC_BSWAP4(x) _byteswap_ulong(x)

#else

#error unsupported compiler/architecture

#endif

/* define HC_BSWAP4
 */

#ifdef HC_BSWAP4

#define HC_GET_BE4(p)     HC_BSWAP4(*(unsigned int *)(p))
#define HC_PUT_BE4(p, v)  do { *(unsigned int *)(p) = HC_BSWAP4(v); } while (0)

#endif

#endif /* tid0312749c542csqvqaudj9q3g02z9fbwf2qthdq7pco1 higherc-bytewise-h */
