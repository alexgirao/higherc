/*
 * Copyright (C) 2010 Alexandre Girao <alexgirao@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/s.h"
#include "higherc/bytewise.h"
#include "higherc/rand.h"

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
		HC_DEF_S(flags);

		s_copyn(flags, "\0\0", 2);  /* initialize
						    * s with at
						    * least 2 bytes */

		for (i=0;i<256;i++) {
			if (!HC_ISASCII(i)) {
				continue;
			}

			flags->len = 0; /* truncate s */
			flags->s[0] = '\0'; /* truncate c string */
			flags->s[1] = '\0'; /* zero second byte too,
					    * for the substring
					    * below */

			if (HC_ISPRINT(i)) {
				s_catz(flags, " print");
			}
			if (HC_ISGRAPH(i)) {
				s_catz(flags, " graph");
			}
			if (HC_ISSPACE(i)) {
				s_catz(flags, " space");
			}
			if (HC_ISBLANK(i)) {
				s_catz(flags, " blank");
			}
			if (HC_ISALNUM(i)) {
				s_catz(flags, " alnum");
			}
			if (HC_ISDIGIT(i)) {
				s_catz(flags, " digit");
			}
			if (HC_ISALPHA(i)) {
				s_catz(flags, " alpha");
				if (HC_ISALPHA_UPPER(i)) {
					s_catz(flags, " alpha_upper");
				} else if (HC_ISALPHA_LOWER(i)) {
					s_catz(flags, " alpha_lower");
				} else {
					HC_FATAL("exhausted");
				}
			}
			if (HC_ISPUNCT(i)) {
				s_catz(flags, " punct");
			}
			if (HC_ISREGEX_META(i)) {
				s_catz(flags, " regex_meta");
			}
			if (HC_ISREGEX_CHAR(i)) {
				s_catz(flags, " regex_char");
			}
			if (HC_ISGLOB(i)) {
				s_catz(flags, " glob");
			}
			if (HC_ISHEXDIGIT(i)) {
				s_catz(flags, " hexdigit");
			}

			flags->s[flags->len] = '\0';

			if (HC_ISPRINT(i)) {
				printf("%3i 0x%.2x %c (flags: %s)\n", i, i, i < 0x20 ? '?' : i, flags->s+1);
			} else if (i == 0x7f) {
				printf("%3i 0x%.2x DEL NOPRINT (flags:%s)\n", i, i, flags->s+1);
			} else {
				printf("%3i 0x%.2x %c NOPRINT (flags:%s)\n", i, i, i < 0x20 ? '?' : i, flags->s+1);
			}
		}

		s_free(flags);
	}
}

static int xx_le(void);
static int xx_be(void);
static int xx_detect(void);
static int (*xx_final)(void) = xx_detect;

int xx(void)
{
	return xx_final();
}

int xx_le(void)
{
	puts("little-endian routine");
	return 1;
}

int xx_be(void)
{
	puts("big-endian routine");
	return 2;
}

int xx_detect(void)
{
	int v = 0xdeadbeef;
	puts("detecting endianess");
	xx_final = *((unsigned char*)&v) == 0xef ? xx_le : xx_be;
	return xx();
}

void len_enc(void)
{
	uint32_t tests[] = {
		0, 1, /*1,*/ 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946,
		17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578,
		5702887, 9227465, 14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296, 433494437, 701408733, 1134903170, 1836311903,
		INT8_MAX, UINT8_MAX, INT16_MAX, UINT16_MAX, INT32_MAX, UINT32_MAX,
		HC_BITMASK(7+7+7+7), HC_BITMASK(7+7+7), HC_BITMASK(7+7), HC_BITMASK(7),
		~HC_BITMASK(7+7+7+7), ~HC_BITMASK(7+7+7), ~HC_BITMASK(7+7), ~HC_BITMASK(7),
		HC_BITMASK(7+7+7+7)+1, HC_BITMASK(7+7+7)+1, HC_BITMASK(7+7)+1, HC_BITMASK(7)+1,
		~HC_BITMASK(7+7+7+7)-1, ~HC_BITMASK(7+7+7)-1, ~HC_BITMASK(7+7)-1, ~HC_BITMASK(7)-1,
		0xdeadbeef
	};
	int tests_len = sizeof(tests) / sizeof(uint32_t);
	uint32_t a[10000];
	int a_len = sizeof(a) / sizeof(a[0]);
	int i;
	uint8_t e[5];
	int e_len, d_len;
	struct entropy re[1];

	/* fillup a with some pre-defined values and random values
	 */

	for (i=0; i<tests_len; i++) {
		a[i] = tests[i];
	}

	time_seed(re);

	for (; i<a_len; i++) {
		a[i] = rand4(re);
	}

	/* tests
	 */

	assert(enc_u4_be(e, 0) == 1);
	assert(enc_u4_be(e, HC_BITMASK(1)) == 1);
	assert(enc_u4_be(e, HC_BITMASK(7)) == 1);
	assert(enc_u4_be(e, HC_BITMASK(7+1)) == 2);
	assert(enc_u4_be(e, HC_BITMASK(7+7)) == 2);
	assert(enc_u4_be(e, HC_BITMASK(7+7+1)) == 3);
	assert(enc_u4_be(e, HC_BITMASK(7+7+7)) == 3);
	assert(enc_u4_be(e, HC_BITMASK(7+7+7+1)) == 4);
	assert(enc_u4_be(e, HC_BITMASK(7+7+7+7)) == 4);
	assert(enc_u4_be(e, HC_BITMASK(7+7+7+7+1)) == 5);
	assert(enc_u4_be(e, 0xffffffff) == 5);

	assert(enc_u4_be(NULL, 0) == 1);
	assert(enc_u4_be(NULL, HC_BITMASK(1)) == 1);
	assert(enc_u4_be(NULL, HC_BITMASK(7)) == 1);
	assert(enc_u4_be(NULL, HC_BITMASK(7+1)) == 2);
	assert(enc_u4_be(NULL, HC_BITMASK(7+7)) == 2);
	assert(enc_u4_be(NULL, HC_BITMASK(7+7+1)) == 3);
	assert(enc_u4_be(NULL, HC_BITMASK(7+7+7)) == 3);
	assert(enc_u4_be(NULL, HC_BITMASK(7+7+7+1)) == 4);
	assert(enc_u4_be(NULL, HC_BITMASK(7+7+7+7)) == 4);
	assert(enc_u4_be(NULL, HC_BITMASK(7+7+7+7+1)) == 5);
	assert(enc_u4_be(NULL, 0xffffffff) == 5);

	for (i=0; i<a_len; i++) {
		bzero(e, 5);
		e_len = enc_u4_be(e, a[i]);
		d_len = 0xdeadbeef; /* dirty `d_len' (offensive programming) */
		assert(a[i] == dec_u4_be(e, &d_len));
		assert(e_len == d_len);
	}

	for (i=0; i<a_len; i++) {
		/* dirty `e' (just being offensive)
		 */
		e[0] = a[(i+0) % a_len];
		e[1] = a[(i+1) % a_len];
		e[2] = a[(i+2) % a_len];
		e[3] = a[(i+3) % a_len];
		e[4] = a[(i+4) % a_len];
		/*
		 */
		e_len = enc_u4_be(e, a[i]);
		d_len = 0xdeadbeef; /* dirty `d_len' (offensive programming) */
		assert(a[i] == dec_u4_be(e, &d_len));
		assert(e_len == d_len);
		assert(e_len == enc_u4_be(NULL, a[i]));
	}

}

void ser_test(void)
{
	/* serialization validation
	 */

#define _B127_1(a0) {a0, a0, 0xff, 0xff, 0xff, 0xff}
#define _B127_2(a1, a0) {((a1) << 7) | (a0), a1 | 0x80, a0, 0xff, 0xff, 0xff}
#define _B127_3(a2, a1, a0) {((a2) << (7+7)) | ((a1) << 7) | (a0), a2 | 0x80, a1 | 0x80, a0, 0xff, 0xff}
#define _B127_4(a3, a2, a1, a0) {((a3) << (7+7+7)) | ((a2) << (7+7)) | ((a1) << 7) | (a0), a3 | 0x80, a2 | 0x80, a1 | 0x80, a0, 0xff}
#define _B127_5(a4, a3, a2, a1, a0) {((a4) << (7+7+7+7)) | ((a3) << (7+7+7)) | ((a2) << (7+7)) | ((a1) << 7) | (a0), a4 | 0x80, a3 | 0x80, a2 | 0x80, a1 | 0x80, a0}

	uint32_t a[][6] = { /* value and it's 5 encoded bytes */
		_B127_1(1),
		_B127_2(1, 1),
		_B127_3(1, 1, 1),
		_B127_4(1, 1, 1, 1),
		_B127_5(1, 1, 1, 1, 1),
		_B127_5(1, 2, 3, 4, 5),
		_B127_5(5, 4, 3, 2, 1)
	};
	int i;
	int a_len = sizeof(a) / sizeof(a[0]);
	uint32_t x, y, *zp;
	uint8_t enc[5], *encp, enc2[5];
	int enc_len;

	for (i=0; i<a_len; i++) {
		enc[0] = 0xff;
		enc[1] = 0xff;
		enc[2] = 0xff;
		enc[3] = 0xff;
		enc[4] = 0xff;
		encp = enc;
		zp = &a[i][1];
		x = a[i][0];
		enc_len = 0;
		while (x) {
			y = x & 0x7f;
			x = x >> 7;
			*encp++ = *zp++;
			enc_len++;
		}
		assert(a[i][0] == dec_u4_be(enc, NULL));
		assert(enc_len == enc_u4_be(enc2, a[i][0]));
		assert(enc_len == enc_u4_be(NULL, a[i][0]));
		assert(b_diff(enc, enc_len, enc2) == 0);
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

	/* binary data
	 */

	{
		unsigned char bin[] = "\x01\x02\x03\xff";

		assert(bin[0] == 1);
		assert(bin[1] == 2);
		assert(bin[2] == 3);
		assert(bin[3] == 0xff);
	}

	/* endianess auto-detect
	 */

	xx();
	xx();
	xx();
	xx();

	/* length encoding
	 */

	printf("0x%08x\n", HC_BITMASK(7+7+7+7));
	printf("0x%08x\n", HC_BITMASK(7+7+7));
	printf("0x%08x\n", HC_BITMASK(7+7+7) + 1);
	printf("0x%08x\n", HC_BITMASK(7+7));
	printf("0x%08x\n", HC_BITMASK(7));

	printf("0x%08x\n", ~HC_BITMASK(7+7+7+7));
	printf("0x%08x\n", ~HC_BITMASK(7+7+7));
	printf("0x%08x\n", ~HC_BITMASK(7+7+7) - 1);
	printf("0x%08x\n", ~HC_BITMASK(7+7));
	printf("0x%08x\n", ~HC_BITMASK(7));

	/*
	 */

	len_enc();

	ser_test();

	puts("ok");

	return 0;
}
