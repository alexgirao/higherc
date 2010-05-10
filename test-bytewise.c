#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"

#define bval(v, pos) (((unsigned char *)&v)[pos])

static void print_int_hex(char *prefix, int v)
{
	printf("%s: 0x%.2x%.2x%.2x%.2x\n", prefix,
	       bval(v, 0), bval(v, 1), bval(v, 2), bval(v, 3));
}

int main(int argc, char **argv)
{
	int x = 0xdeadbeef;
	unsigned int y = x;

	print_int_hex("x         ", x);
	print_int_hex("y         ", y);
	print_int_hex("bswap(x)  ", HC_BSWAP4(x));
	print_int_hex("bswap(y)  ", HC_BSWAP4(y));
	print_int_hex("bswap(x)  ", HC_BSWAP4(x));
	print_int_hex("bswap(y)  ", HC_BSWAP4(y));
	print_int_hex("get_be4(x)", HC_GET_BE4(&x));
	print_int_hex("get_be4(y)", HC_GET_BE4(&y));

	int i;
	unsigned j;

	HC_PUT_BE4(&i, x);
	HC_PUT_BE4(&j, y);
	print_int_hex("x         ", i);
	print_int_hex("y         ", j);

	return 0;
}
