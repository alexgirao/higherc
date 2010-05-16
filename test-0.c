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

#define myassert(fmt, val, check) assert((val) == (check)); printf(fmt, val);

int main(int argc, char **argv)
{
	char c = 0xef;

	myassert("1: %8x\n", (int)c, 0xffffffef);              // two's complement aware
	myassert("2: %8x\n", (int)(char)c, 0xffffffef);        // likewise
	myassert("3: %8x\n", (unsigned int)c, 0xffffffef);     // likewise (sign-bit is carried on type change)
	myassert("4: %8x\n", (unsigned int)(unsigned char)c, 0xef);        // bitwise ;-)
	myassert("5: %8x\n", (int)(unsigned char)c, 0xef);    // bitwise ;-)
	myassert("6: %8x\n", c & 0xff, 0xef);                 // explicity bit selection
	myassert("7: %8x\n", c << 8, 0xffffef00);             // bit shift to the left consider sign
	myassert("8: %8x\n", (c << 8) >> 8, 0xffffffef);      // bit shift to the right consider sign

	return 0;
}
