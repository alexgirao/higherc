#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"
#include "higherc/byte.h"
#include "higherc/bytewise.h"
#include "higherc/stralloc.h"
#include "higherc/str.h"
#include "higherc/rand.h"
#include "higherc/fatal.h"
#include "higherc/n.h"

#define DEBUG_N(n) do {							\
		printf(__FILE__ ":%i: ", __LINE__);			\
		print_n(#n " [", n, "]");				\
		printf(" (len: %i, a: %i)\n", (n)->len, (n)->a);	\
		fflush(stdout);						\
	} while (0)

/*
 * r[xl+1] = x[xl] * d
 */
static int D_mul(hcns(h)* x, int xl, hcns(h) d, hcns(h)* r)
{
	hcns(h) c = 0;
	int rl = xl;
	hcns(h) p;

	switch(d) {
	case 0:
		HC_ZERO(r, 1);
		return 1;
	case 1:
		if (x != r)
			HC_MOVE(r, x, xl);
		return xl;
	case 2:
		while(xl--) {
			p = *x;
			HC_ADD_C(p, p, c, p);
			*r++ = p;
			x++;
		}
		break;
	default:
		while(xl--) {
			DMULc(d, *x, c, p);
			*r++ = p;
			x++;
		}
		break;
	}
	if (c == 0)
		return rl;
	*r = c;
	return rl+1;
}

static void print_n(char *prefix, struct hcns(n) *n, char *suffix)
{
	struct hcns(s) s = HC_NULL_S;

	assert(n->d != NULL);
	assert(n->len > 0);

	hcns(s_catz)(&s, prefix);
	hcns(n_as_hex)(n, &s);
	hcns(s_catz)(&s, suffix);

	HC_SAFE_CSTR(&s);

	printf("%s", s.s);
}

static int n_cmp_hex(struct hcns(n) *n, char *hex)
{
	struct hcns(s) s = HC_NULL_S;
	hcns(n_as_hex)(n, &s);
	HC_SAFE_CSTR(&s);
	return hcns(s_sdiff)(&s, hex);
}

static void test_MUL()
{
	hcns(h) a[2];
	hcns(h) b;
	hcns(h) c;

	b = HC_HALF_OFFENSE;
	c = HC_HALF_OFFENSE;

	DMUL(b, c, a[1], a[0]);

	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xc1b0 && a[0] == 0x80e9);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul
	 */

	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = HC_HALF_OFFENSE;
	a[0] = HC_HALF_OFFENSE;

	DMUL(b, c, a[1], a[0]);

	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0001);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul_c without carry
	 */
		
	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = 0;  /* carry goes here */
	a[0] = HC_HALF_OFFENSE;
		
	DMULc(b, c, a[1], a[0]);
	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0001);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul_c with carry
	 */
		
	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = 1;  /* carry goes here */
	a[0] = HC_HALF_OFFENSE;
		
	DMULc(b, c, a[1], a[0]);
	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0002);
	} else {
		HC_FATAL("not implemented");
	}

	/* mul_c with carry
	 */
		
	b = HC_HALF_MAX;
	c = HC_HALF_MAX;
		
	a[1] = 2;  /* carry goes here */
	a[0] = HC_HALF_OFFENSE;
		
	DMULc(b, c, a[1], a[0]);
	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0003);
	} else {
		HC_FATAL("not implemented");
	}
}

#if (HC_HALF_BYTES == 2)

void hcns(n_set_u4)(struct hcns(n) *n, hcns(u4) v)
{
	if (!hcns(n_alloc)(n, 2)) {
		HC_FATAL("n_alloc(%p, %i) failed", n, 2);
	}
	n->d[0] = HC_LOW(v);
	n->d[1] = HC_HIGH(v);
	n->len = 2;
}

void hcns(n_load_be1)(struct hcns(n) *r, void *x, int len)
{
	int i, ndigits = len / HC_HALF_BYTES;
	hcns(u1) *v = x;

	if (!hcns(n_alloc)(r, ndigits)) {
		HC_FATAL("n_alloc(%p, %i) failed", r, ndigits);
	}
	r->len = ndigits;

	for (i=ndigits-1; i>=0; i--) {
		r->d[i] = v[0] << 8 | v[1];
		v += 2;
	}
}

void hcns(n_load_hex)(struct hcns(n) *r, char *hex, int n)
{
	int i, ndigits = (n / 2) / HC_HALF_BYTES;
	hcns(u1) *v = (hcns(u1)*) hex;

	if (n % 2) {
		/* n must be even, so we have a byte for every 2
		 * nibbles
		 */
		HC_ZERO(r->d, r->len);
		return;
	}

	if (!hcns(n_alloc)(r, ndigits)) {
		HC_FATAL("n_alloc(%p, %i) failed", r, ndigits);
	}
	r->len = ndigits;

	for (i=ndigits-1; i>=0; i--) {
		r->d[i] =
			HC_HEX_VALUE(v[0]) << 12 |
			HC_HEX_VALUE(v[1]) << 8 |
			HC_HEX_VALUE(v[2]) << 4 |
			HC_HEX_VALUE(v[3]);
		v += 4;
	}
}

void hcns(n_as_hex)(struct hcns(n) *n, struct hcns(s) *s)
{
	int i, ndigits = n->len;
	int s_start_len = s->len;

	hcns(s_alloc)(s, s->len + n->len * (HC_HALF_BYTES * 2)); /* each byte has 2 hex digits */

	i = ndigits - 1;

	/* skip leading zeroes
	 */

	while (i>=0) {
		char t[4];
		hcns(h) d = n->d[i--];

		if (d) {
			t[0] = HC_HEX_DIGIT(d >> 12);
			t[1] = HC_HEX_DIGIT(d >> 8);
			t[2] = HC_HEX_DIGIT(d >> 4);
			t[3] = HC_HEX_DIGIT(d);

			int j = 0;

			while (j < 4 && t[j] == '0') j++;

			hcns(s_catn)(s, t + j, 4 - j);

			break;
		}
	}

	/* output remaining digits
	 */

	while (i>=0) {
		char t[4];
		hcns(h) d = n->d[i--];

		t[0] = HC_HEX_DIGIT(d >> 12);
		t[1] = HC_HEX_DIGIT(d >> 8);
		t[2] = HC_HEX_DIGIT(d >> 4);
		t[3] = HC_HEX_DIGIT(d);

		hcns(s_catn)(s, t, 4);
	}

	if (s_start_len == s->len) {
		hcns(s_catn)(s, "0", 1);
	}
}

#else
#error not implemented
#endif

static void test_hex_in_out()
{
	char hexstr[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	assert(sizeof(hexstr) == 40);

	{
		struct hcns(n) a = HC_NULL_N;
		struct hcns(s) s = HC_NULL_S;
		unsigned char valstr[sizeof(hexstr) / 2];
	
		HC_GET_HEX(valstr, sizeof(valstr), hexstr);
		assert(valstr[0] == 0xda && valstr[sizeof(valstr) - 1] == 0x09);

		hcns(n_load_be1)(&a, valstr, sizeof(valstr));
		if (HC_HALF_BITS == 16) {
			assert(a.d[a.len-1] == 0xda39 && a.d[a.len-2] == 0xa3ee && a.d[1] == 0xafd8 && a.d[0] == 0x0709);
		} else {
			HC_FATAL("not implemented");
		}

		s.len = 0;
		hcns(n_as_hex)(&a, &s);
		HC_SAFE_CSTR(&s);

		assert(hcns(sdiffn)(s.s, hexstr, sizeof(hexstr)) == 0);
	}

	{
		struct hcns(n) a = HC_NULL_N;
		struct hcns(s) s = HC_NULL_S;

		hcns(n_load_hex)(&a, hexstr, sizeof(hexstr));
		if (HC_HALF_BITS == 16) {
			assert(a.d[a.len-1] == 0xda39 && a.d[a.len-2] == 0xa3ee && a.d[1] == 0xafd8 && a.d[0] == 0x0709);
		} else {
			HC_FATAL("not implemented");
		}

		s.len = 0;
		hcns(n_as_hex)(&a, &s);
		HC_SAFE_CSTR(&s);

		//assert(hcns(s_sdiff)(&s, hexstr) == 0);
		assert(hcns(sdiffn)(s.s, hexstr, sizeof(hexstr)) == 0);
	}

	{
		struct hcns(n) a[1] = {HC_NULL_N};

		assert(sizeof(a) == sizeof(struct hcns(n)));

		hcns(n_set_u4)(a, 0x00000001U);
		assert(n_cmp_hex(a, "1") == 0);

		hcns(n_set_u4)(a, 0x00000012U);
		assert(n_cmp_hex(a, "12") == 0);

		hcns(n_set_u4)(a, 0x00000123U);
		assert(n_cmp_hex(a, "123") == 0);

		hcns(n_set_u4)(a, 0x00001234U);
		assert(n_cmp_hex(a, "1234") == 0);

		hcns(n_set_u4)(a, 0x00012345U);
		assert(n_cmp_hex(a, "12345") == 0);

		hcns(n_set_u4)(a, 0x00123456U);
		assert(n_cmp_hex(a, "123456") == 0);

		hcns(n_set_u4)(a, 0x01234567U);
		assert(n_cmp_hex(a, "1234567") == 0);

		hcns(n_set_u4)(a, 0x12345678U);
		assert(n_cmp_hex(a, "12345678") == 0);

		hcns(n_set_u4)(a, 0x23456789U);
		assert(n_cmp_hex(a, "23456789") == 0);

		hcns(n_set_u4)(a, 0x34567890U);
		assert(n_cmp_hex(a, "34567890") == 0);

		hcns(n_set_u4)(a, 0x45678901U);
		assert(n_cmp_hex(a, "45678901") == 0);

		hcns(n_set_u4)(a, 0x56789012U);
		assert(n_cmp_hex(a, "56789012") == 0);

		hcns(n_set_u4)(a, 0xdeadbeefU);
		assert(n_cmp_hex(a, "deadbeef") == 0);
	}
}

static void test_mul1()
{
	char a_hex[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	struct hcns(n) a = HC_NULL_N;
	struct hcns(n) r = HC_NULL_N;

	hcns(n_load_hex)(&a, a_hex, sizeof(a_hex));

	hcns(n_alloc)(&r, a.len + 1);

	r.len = D_mul(a.d, a.len, HC_H(0), r.d);

	//print_n("r = [", &r, "]\n");

	assert(n_cmp_hex(&r, "0") == 0);
}

int main(int argc, char **argv)
{

	assert(sizeof(hcns(u1)) == 1);
	assert(sizeof(hcns(u2)) == 2);
	assert(sizeof(hcns(u4)) == 4);
//	assert(sizeof(hcns(u8)) == 8);   // future

	test_MUL();  // DMUL
	test_hex_in_out();
	test_mul1(); // D_mul

	return 0;
}
