#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/s.h"
#include "higherc/bytewise.h"
#include "higherc/fatal.h"

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
	assert(HC_ISASCII(0));
	assert(HC_ISASCII(127));
	assert(!HC_ISASCII(128));
	assert(!HC_ISASCII(255));
	assert(!HC_ISASCII(256));

	assert(HC_ISDIGIT('0') && !HC_ISALPHA('0'));
	assert(HC_ISDIGIT('9') && !HC_ISALPHA('9'));
	assert(!HC_ISDIGIT('a') && HC_ISALPHA('a'));
	assert(!HC_ISDIGIT('A') && HC_ISALPHA('A'));
	assert(!HC_ISDIGIT('f') && HC_ISALPHA('f'));
	assert(!HC_ISDIGIT('F') && HC_ISALPHA('F'));

	assert(
		HC_ISALNUM('0') && 
		HC_ISALNUM('9') && 
		HC_ISALNUM('a') && 
		HC_ISALNUM('f') && 
		HC_ISALNUM('z') && 
		HC_ISALNUM('A') && 
		HC_ISALNUM('F') && 
		HC_ISALNUM('Z') && 
		!HC_ISALNUM('.') && 
		!HC_ISALNUM('*')
		);

	assert(HC_ISSPACE(' '));
	assert(HC_ISSPACE('\r'));
	assert(HC_ISSPACE('\n'));
	assert(HC_ISSPACE('\t'));

	assert(HC_TOUPPER('0') == '0');
	assert(HC_TOUPPER('9') == '9');
	assert(HC_TOLOWER('0') == '0');
	assert(HC_TOLOWER('9') == '9');

	assert(HC_TOUPPER('A') == 'A');
	assert(HC_TOUPPER('F') == 'F');
	assert(HC_TOUPPER('Z') == 'Z');

	assert(HC_TOLOWER('a') == 'a');
	assert(HC_TOLOWER('f') == 'f');
	assert(HC_TOLOWER('z') == 'z');

	assert(HC_TOUPPER('a') == 'A');
	assert(HC_TOUPPER('f') == 'F');
	assert(HC_TOUPPER('z') == 'Z');

	assert(HC_TOLOWER('A') == 'a');
	assert(HC_TOLOWER('F') == 'f');
	assert(HC_TOLOWER('Z') == 'z');

	{
		int i;
		struct hcns(s) flags = HC_NULL_S;

		hcns(s_copyn)(&flags, "\0\0", 2);  /* initialize
						    * s with at
						    * least 2 bytes */

		for (i=0;i<256;i++) {
			if (!HC_ISASCII(i)) {
				continue;
			}

			flags.len = 0; /* truncate s */
			flags.s[0] = '\0'; /* truncate c string */
			flags.s[1] = '\0'; /* zero second byte too,
					    * for the substring
					    * below */

			if (HC_ISPRINT(i)) {
				hcns(s_catz)(&flags, " print");
			}
			if (HC_ISGRAPH(i)) {
				hcns(s_catz)(&flags, " graph");
			}
			if (HC_ISSPACE(i)) {
				hcns(s_catz)(&flags, " space");
			}
			if (HC_ISBLANK(i)) {
				hcns(s_catz)(&flags, " blank");
			}
			if (HC_ISALNUM(i)) {
				hcns(s_catz)(&flags, " alnum");
			}
			if (HC_ISDIGIT(i)) {
				hcns(s_catz)(&flags, " digit");
			}
			if (HC_ISALPHA(i)) {
				hcns(s_catz)(&flags, " alpha");
				if (HC_ISALPHA_UPPER(i)) {
					hcns(s_catz)(&flags, " alpha_upper");
				} else if (HC_ISALPHA_LOWER(i)) {
					hcns(s_catz)(&flags, " alpha_lower");
				} else {
					HC_FATAL("exhausted");
				}
			}
			if (HC_ISPUNCT(i)) {
				hcns(s_catz)(&flags, " punct");
			}
			if (HC_ISREGEX_META(i)) {
				hcns(s_catz)(&flags, " regex_meta");
			}
			if (HC_ISREGEX_CHAR(i)) {
				hcns(s_catz)(&flags, " regex_char");
			}
			if (HC_ISGLOB(i)) {
				hcns(s_catz)(&flags, " glob");
			}

			flags.s[flags.len] = '\0';

			if (HC_ISPRINT(i)) {
				printf("%3i 0x%.2x %c (flags: %s)\n", i, i, i < 0x20 ? '?' : i, flags.s+1);
			} else if (i == 0x7f) {
				printf("%3i 0x%.2x DEL NOPRINT (flags:%s)\n", i, i, flags.s+1);
			} else {
				printf("%3i 0x%.2x %c NOPRINT (flags:%s)\n", i, i, i < 0x20 ? '?' : i, flags.s+1);
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
