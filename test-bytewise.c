#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"

#define bval(v, pos) (((unsigned char *)&v)[pos])

static void check_int(char *prefix, int v, int b3, int b2, int b1, int b0)
{
	printf("%s: 0x%.2x%.2x%.2x%.2x\n", prefix,
	       bval(v, 0), bval(v, 1), bval(v, 2), bval(v, 3));

	assert((v & 0xff000000) >> 24 == b3);
	assert((v & 0x00ff0000) >> 16 == b2);
	assert((v & 0x0000ff00) >> 8 == b1);
	assert((v & 0x000000ff)  == b0);
}

#define BE_0XDEADBEEF 0xde, 0xad, 0xbe, 0xef
#define LE_0XDEADBEEF 0xef, 0xbe, 0xad, 0xde

int main(int argc, char **argv)
{
	int x = 0xdeadbeef;
	unsigned int y = x;

	check_int("x         ", x, 0xde, 0xad, 0xbe, 0xef);
	check_int("y         ", y, 0xde, 0xad, 0xbe, 0xef);

	check_int("bswap(x)  ", HC_BSWAP4(x), LE_0XDEADBEEF);
	check_int("bswap(y)  ", HC_BSWAP4(y), LE_0XDEADBEEF);

	check_int("get_be4(x)", HC_GET_BE4(&x), LE_0XDEADBEEF);
	check_int("get_be4(y)", HC_GET_BE4(&y), LE_0XDEADBEEF);

	int i;
	unsigned int j;

	HC_PUT_BE4(&i, x);
	HC_PUT_BE4(&j, y);
	check_int("put_be4(x)", i, LE_0XDEADBEEF);
	check_int("put_be4(y)", j, LE_0XDEADBEEF);

	return 0;
}
