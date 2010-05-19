#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"
#include "higherc/rand.h"
#include "higherc/stralloc.h"
#include "higherc/n.h"

#define myassert(fmt, val, check) assert((val) == (check)); printf(fmt, val);

int main(int argc, char **argv)
{
	char c = 0xef;

	assert(sizeof(char) == 1);

	/* type conversion
	 */

	if ((int)((char)-1) == (int)-1) {
		/* char defaults to signed char
		 */
		printf("char type is signed\n");
		myassert("1: %8x\n", (int)c, 0xffffffef);              // two's complement aware
		myassert("2: %8x\n", (int)(char)c, 0xffffffef);        // likewise
		myassert("3: %8x\n", (unsigned int)c, 0xffffffef);     // likewise (sign-bit is carried on type change)
		myassert("4: %8x\n", (unsigned int)(unsigned char)c, 0xef);        // bitwise ;-)
		myassert("5: %8x\n", (int)(unsigned char)c, 0xef);    // bitwise ;-)
		myassert("6: %8x\n", c & 0xff, 0xef);                 // explicity bit selection
		myassert("7: %8x\n", c << 8, 0xffffef00);             // bit shift to the left consider sign
		myassert("8: %8x\n", (c << 8) >> 8, 0xffffffef);      // bit shift to the right consider sign
	} else {
		printf("char type is unsigned\n");
	}

	/* integer overflow
	 */

	assert(sizeof(int) == 4);

	unsigned int a, b, r;

	a = 0xffffffff;
	b = 1234;

	printf("a = %08x (%i), b = %08x (%i)\n", a, a, b, b);

	/* when overflow happens in addition in a two's complement
	 * system, result is always less than any operands
	 */

	r = a + b;
	assert(r < a && r < b);
	printf("%08x %i (overflow happend? %i)\n", r, r, r < a);

	r = b + a;
	assert(r < a && r < b);
	printf("%08x %i (overflow happend? %i)\n", r, r, r < a);

	/* when overflow happens in subtraction in a two's complement
	 * system, result is ...
	 */

	r = a - b;
	printf("%08x (%i) = %08x (%i) - %08x (%i)\n", r, r, a, a, b, b);

	r = b - a;
	printf("%08x (%i) = %08x (%i) - %08x (%i)\n", r, r, b, b, a, a);

	return 0;
}
