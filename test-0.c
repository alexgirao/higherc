#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/list.h"
#include "higherc/rand.h"
#include "higherc/s.h"
#include "higherc/n.h"

#define myassert(fmt, val, check) assert((val) == (check)); printf(fmt, val);

int main(int argc, char **argv)
{
	signed char sc = 0xef;

	assert(sizeof(char) == 1);

	/* type conversion
	 */

	if ((int)((signed char)-1) == (int)-1) {
		myassert("1: %8x\n", (int)sc, 0xffffffef);                // two's complement aware
		myassert("2: %8x\n", (int)(signed char)sc, 0xffffffef);   // likewise
		myassert("3: %8x\n", (unsigned int)sc, 0xffffffef);       // likewise (sign-bit is carried on type change)
		myassert("4: %8x\n", (unsigned int)(unsigned char)sc, 0xef);        // bitwise ;-)
		myassert("5: %8x\n", (int)(unsigned char)sc, 0xef);    // bitwise ;-)
		myassert("6: %8x\n", sc & 0xff, 0xef);                 // explicity bit selection
		myassert("7: %8x\n", sc << 8, 0xffffef00);             // bit shift to the left consider sign
		myassert("8: %8x\n", (sc << 8) >> 8, 0xffffffef);      // bit shift to the right consider sign
	} else {
		printf("no signed char support\n");
	}

	/* integer overflow
	 */

	assert(sizeof(int) == 4);

	unsigned int a, b, c, r;

	a = 0xffffffff;
	b = 1234;
	c = 4321;

	printf("a = %08x (%i), b = %08x (%i)\n", a, a, b, b);

	/* when overflow happens in addition in a two's complement
	 * system, result is always less than any operands
	 */

	r = a + b;
	assert(r < a && r < b);
	printf("%08x (%i) = %08x (%i) + %08x (%i) (overflow happend? %i)\n", r, r, a, a, b, b, r < a);

	r = b + c;
	assert(r >= b && r >= c);
	printf("%08x (%i) = %08x (%i) + %08x (%i) (overflow happend? %i)\n", r, r, b, b, c, c, r < b);

	/* when overflow happens in subtraction in a two's complement
	 * system, result is ...
	 */

	r = a - b;
	assert(r <= a);
	printf("%08x (%i) = %08x (%i) - %08x (%i) (overflow happend? %i)\n", r, r, a, a, b, b, r > a);

	r = b - a;
	assert(r > b);
	printf("%08x (%i) = %08x (%i) - %08x (%i) (overflow happend? %i)\n", r, r, b, b, a, a, r > b);

	/* pointer arithmetic in types greater than char
	 */

	{
		int *a, *b;

		a = NULL;
		b = a + 1;

		assert(sizeof(*a) > 1);
		assert(b - a == 1); /* pointer arithmetic consider type size */
		assert((char*)b - (char*)a == sizeof(int));
	}

	/* two's complement of int
	 */

	a = -2;
	b = ~a;
	assert(b == 1);
	printf("a: %i, b: %i\n", a, b);

	a = -1;
	b = ~a;
	assert(b == 0);
	printf("a: %i, b: %i\n", a, b);

	a = 0;
	b = ~a;
	assert(b == -1);
	printf("a: %i, b: %i\n", a, b);

	a = 1;
	b = ~a;
	assert(b == -2);
	printf("a: %i, b: %i\n", a, b);

	a = 0x7fffffff; /* greatest unsigned value for 32-bit architectures */
	b = ~a;
	assert(b == 0x80000000);
	printf("a: %i, b: %i\n", a, b);

	return 0;
}
