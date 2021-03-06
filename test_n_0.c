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

#include "higherc/higherc.h"
#include "higherc/list.h"
#include "higherc/byte.h"
#include "higherc/bytewise.h"
#include "higherc/s.h"
#include "higherc/str.h"
#include "higherc/rand.h"
#include "higherc/n.h"

#define DEBUG_N(n) do {							\
		printf(__FILE__ ":%i: ", __LINE__);			\
		print_n(#n " [", n, "]");				\
		printf(" (d: %p, len: %i, a: %i)\n", (n)->d, (n)->len, (n)->a); \
		fflush(stdout);						\
	} while (0)

static void print_n(char *prefix, struct n *n, char *suffix)
{
	struct s s = HC_NULL_S;

	s_catz(&s, prefix);
	n_as_hex(n, &s);
	s_catz(&s, suffix);

	HC_SAFE_CSTR(&s);

	printf("%s", s.s);

	s_free(&s);
}

static void test_hex_in_out()
{
	char hexstr[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	assert(sizeof(hexstr) == 40);

	{
		struct n a = HC_NULL_N;
		struct s s = HC_NULL_S;
		unsigned char valstr[sizeof(hexstr) / 2];
	
		HC_GET_HEX(valstr, sizeof(valstr), hexstr);
		assert(valstr[0] == 0xda && valstr[sizeof(valstr) - 1] == 0x09);

		n_load_be1(&a, valstr, sizeof(valstr));
		if (HC_HALF_BITS == 16) {
			assert(a.d[a.len-1] == 0xda39 && a.d[a.len-2] == 0xa3ee && a.d[1] == 0xafd8 && a.d[0] == 0x0709);
		} else {
			HC_FATAL("not implemented");
		}

		s.len = 0;
		assert(n_as_hex(&a, &s) == 40);
		HC_SAFE_CSTR(&s);

		assert(sdiffn(s.s, hexstr, sizeof(hexstr)) == 0);

		s_free(&s);
		n_free(&a);
	}

	{
		struct n a = HC_NULL_N;
		struct s s = HC_NULL_S;

		n_load_hex(&a, hexstr, sizeof(hexstr));
		if (HC_HALF_BITS == 16) {
			assert(a.d[a.len-1] == 0xda39 && a.d[a.len-2] == 0xa3ee && a.d[1] == 0xafd8 && a.d[0] == 0x0709);
		} else {
			HC_FATAL("not implemented");
		}

		s.len = 0;
		assert(n_as_hex(&a, &s) == 40);
		HC_SAFE_CSTR(&s);

		assert(sdiffn(s.s, hexstr, sizeof(hexstr)) == 0);

		s_free(&s);
		n_free(&a);
	}

	{
		struct n a[1] = {HC_NULL_N};

		assert(sizeof(a) == sizeof(struct n));

		n_set_u4(a, 0x00000000U);
		assert(n_cmp_hexz(a, "0") == 0);
		assert(n_cmp_hexz(a, "00") == 0);
		assert(n_cmp_hexz(a, "000") == 0);

		n_set_u4(a, 0x00000001U);
		assert(n_cmp_hexz(a, "1") == 0);
		assert(n_cmp_hexz(a, "01") == 0);
		assert(n_cmp_hexz(a, "001") == 0);

		n_set_u4(a, 0x00000012U);
		assert(n_cmp_hexz(a, "12") == 0);
		assert(n_cmp_hexz(a, "012") == 0);
		assert(n_cmp_hexz(a, "0012") == 0);

		n_set_u4(a, 0x00000123U);
		assert(n_cmp_hexz(a, "123") == 0);
		assert(n_cmp_hexz(a, "0123") == 0);
		assert(n_cmp_hexz(a, "00123") == 0);

		n_set_u4(a, 0x00001234U);
		assert(n_cmp_hexz(a, "1234") == 0);

		n_set_u4(a, 0x00012345U);
		assert(n_cmp_hexz(a, "12345") == 0);

		n_set_u4(a, 0x00123456U);
		assert(n_cmp_hexz(a, "123456") == 0);

		n_set_u4(a, 0x01234567U);
		assert(n_cmp_hexz(a, "1234567") == 0);

		n_set_u4(a, 0x12345678U);
		assert(n_cmp_hexz(a, "12345678") == 0);

		n_set_u4(a, 0x23456789U);
		assert(n_cmp_hexz(a, "23456789") == 0);

		n_set_u4(a, 0x34567890U);
		assert(n_cmp_hexz(a, "34567890") == 0);

		n_set_u4(a, 0x45678901U);
		assert(n_cmp_hexz(a, "45678901") == 0);

		n_set_u4(a, 0x56789012U);
		assert(n_cmp_hexz(a, "56789012") == 0);

		n_set_u4(a, 0xdeadbeefU);
		assert(n_cmp_hexz(a, "deadbeef") == 0);

		n_free(a);
	}

	{
		struct n a[1] = {HC_NULL_N};
		char *s;

		s="0"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="00"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="1"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="01"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="001"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="12"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="012"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="0012"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="123"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="1234"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="12345"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="00012345"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="000012345"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="0000012345"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="123456"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		s="1234567"; n_load_hexz(a, s);
		assert(n_cmp_hexz(a, s) == 0);

		n_free(a);
	}
}

static void test_muln()
{
	char *tests0[] = {
		#include "bignum.escript--c_verify"
		NULL
	};
	char **tests = tests0;

	void verify(char *a_hex, char *b_hex, char *expected)
	{
		struct n a[1] = {HC_NULL_N};
		struct n b[1] = {HC_NULL_N};
		struct n r[1] = {HC_NULL_N};
		int max_digits;

		n_load_hexz(a, a_hex);
		n_load_hexz(b, b_hex);

		assert(n_cmp_hexz(a, a_hex) == 0);
		assert(n_cmp_hexz(b, b_hex) == 0);

		n_alloc(r, a->len + b->len);

		max_digits = a->len > b->len ? a->len : b->len;

		/* r = a * b
		 */

		if (b->len == 1) {
			r->len = D_mul(a->d, a->len, b->d[0], r->d);
		} else if (a->len == 1) {
			r->len = D_mul(b->d, b->len, a->d[0], r->d);
		} else {
			HC_ZERO_DIGITS(r->d, max_digits);
			r->len = I_mul(a->d, a->len, b->d, b->len, r->d);
		}

		assert(n_cmp_hexz(r, expected) == 0);

		/* r = b * a
		 */

		HC_ZERO_DIGITS(r->d, max_digits);
		r->len = I_mul(b->d, b->len, a->d, a->len, r->d);
		assert(n_cmp_hexz(r, expected) == 0);

		n_free(r);
		n_free(a);
		n_free(b);
	}

	while (*tests) {
		char *a = *tests++;
		char *b = *tests++;
		char *e = *tests++;

		assert(a);
		assert(b);
		assert(e);

		verify(a, b, e);
		verify(b, a, e);   /* double inversion */
	}
}

static void test_divn()
{
	char *tests0[] = {
		#include "bignum2.escript--c_verify"
		NULL
	};
	char **tests = tests0;

	void verify(char *a_hex, char *b_hex, char *eq, char *er)
	{
		struct n a[1] = {HC_NULL_N};
		struct n b[1] = {HC_NULL_N};
		struct n q[1] = {HC_NULL_N};
		struct n r[1] = {HC_NULL_N};

		n_load_hexz(a, a_hex);
		n_load_hexz(b, b_hex);

		assert(n_cmp_hexz(a, a_hex) == 0);
		assert(n_cmp_hexz(b, b_hex) == 0);

		assert(I_comp(a->d, a->len, b->d, b->len) >= 0);

		n_alloc(q, a->len);

		/* note: I_div zeroes q automatically
		 */

		/* divide preserving a
		 */

		if (b->len == 1) {
			/* how to optimize if b (divisor) is only 1 digit length
			 */

			n_alloc(r, 1);
			r->len = 1;
			q->len = D_div(
				a->d, a->len,
				b->d[0],
				q->d,
				r->d);
		} else {
			n_alloc(r, a->len);
			q->len = I_div(
				a->d, a->len,
				b->d, b->len,
				q->d,
				r->d,
				&r->len
				);
		}

		assert(n_cmp_hexz(a, a_hex) == 0);
		assert(n_cmp_hexz(b, b_hex) == 0);
		assert(n_cmp_hexz(q, eq) == 0);
		assert(n_cmp_hexz(r, er) == 0);

		n_free(r);

		/* divide putting r in a (overwrite a!)
		 */

		if (b->len > 1) {
			q->len = I_div(
				a->d, a->len,
				b->d, b->len,
				q->d,
				a->d,
				&a->len
				);
			
			assert(n_cmp_hexz(b, b_hex) == 0);
			assert(n_cmp_hexz(q, eq) == 0);
			assert(n_cmp_hexz(a, er) == 0);
		}

		n_free(q);
		n_free(a);
		n_free(b);
	}

	while (*tests) {
		char *a = *tests++;
		char *b = *tests++;
		char *eq = *tests++;
		char *er = *tests++;

		assert(a);
		assert(b);
		assert(eq);
		assert(er);

		verify(a, b, eq, er);  /* a >= b always */
	}
}

int main(int argc, char **argv)
{
	if (0) {
		struct n _unused = HC_NULL_N;
		DEBUG_N(&_unused);
	}

	assert(sizeof(uint8_t) == 1);
	assert(sizeof(uint16_t) == 2);
	assert(sizeof(uint32_t) == 4);
//	assert(sizeof(u8) == 8);   // future

	test_hex_in_out();
	test_muln();
	test_divn();

	puts("ok");

	return 0;
}
