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

static void test_ctype()
{
	assert(isascii(0));
	assert(isascii(127));
	assert(!isascii(128));
	assert(!isascii(255));
	assert(!isascii(256));

	assert(isdigit('0') && !isalpha('0'));
	assert(isdigit('9') && !isalpha('9'));
	assert(!isdigit('a') && isalpha('a'));
	assert(!isdigit('A') && isalpha('A'));
	assert(!isdigit('f') && isalpha('f'));
	assert(!isdigit('F') && isalpha('F'));

	assert(
		isalnum('0') && 
		isalnum('9') && 
		isalnum('a') && 
		isalnum('f') && 
		isalnum('z') && 
		isalnum('A') && 
		isalnum('F') && 
		isalnum('Z') && 
		!isalnum('.') && 
		!isalnum('*')
		);

	assert(isspace(' '));
	assert(isspace('\r'));
	assert(isspace('\n'));
	assert(isspace('\t'));

	assert(toupper('0') == '0');
	assert(toupper('9') == '9');
	assert(tolower('0') == '0');
	assert(tolower('9') == '9');

	assert(toupper('A') == 'A');
	assert(toupper('F') == 'F');
	assert(toupper('Z') == 'Z');

	assert(tolower('a') == 'a');
	assert(tolower('f') == 'f');
	assert(tolower('z') == 'z');

	assert(toupper('a') == 'A');
	assert(toupper('f') == 'F');
	assert(toupper('z') == 'Z');

	assert(tolower('A') == 'a');
	assert(tolower('F') == 'f');
	assert(tolower('Z') == 'z');

	{
		int i;
		for (i=0;i<256;i++) {
			if (!isascii(i)) {
				continue;
			}
			if (isalpha(i)) {
				if ((i & 0x20) == 0) {
					printf("0x%.2x %c (lower case: %c)\n", i, i, tolower(i));
				} else {
					printf("0x%.2x %c (upper case: %c)\n", i, i, toupper(i));
				}
			} else if (isdigit(i)) {
				printf("0x%.2x %c\n", i, i);
			} else if (isgraph(i)) {
				printf("0x%.2x %c (graph)\n", i, i);
			}
		}
	}
}

int main(int argc, char **argv)
{
	int x = 0xdeadbeef;
	unsigned int y = x;
	unsigned int endian0 = 1;
	int is_litle_endian = *((unsigned char *)(&endian0));

	/*
	 */

	assert(sizeof(int) == 4);
	assert(is_litle_endian == 1);

	/* network byte order/big endian
	 */

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

	/* hex base
	 */

	printf("hex value: %i .. %i %i .. %i\n", HC_HEX_VALUE('0'), HC_HEX_VALUE('9'), HC_HEX_VALUE('a'), HC_HEX_VALUE('f'));
	printf("hex value: %i .. %i %i .. %i\n", HC_HEX_VALUE('0'), HC_HEX_VALUE('9'), HC_HEX_VALUE('A'), HC_HEX_VALUE('F'));
	printf("hex value: %i .. %i %i .. %i\n", HC_HEX_VALUE(0), HC_HEX_VALUE(127), HC_HEX_VALUE(255), HC_HEX_VALUE(1234));
	printf("hex digit: %c .. %c %c .. %c\n", HC_HEX_DIGIT(0), HC_HEX_DIGIT(9), HC_HEX_DIGIT(10), HC_HEX_DIGIT(15));

	/*
	 */

	test_ctype();

	return 0;
}
