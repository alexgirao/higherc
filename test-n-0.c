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

static void show_2_halves(hcns(h) a[2])
{
	printf("a[1] == 0x" HC_FMT_H " && a[0] == 0x" HC_FMT_H "\n", a[1], a[0]);
}

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

static void test_MUL()
{
	hcns(h) a[2];
	hcns(h) b;
	hcns(h) c;

	b = HC_HALF_OFFENSE;
	c = HC_HALF_OFFENSE;

	DMUL(b, c, a[1], a[0]);
	show_2_halves(a);

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
	show_2_halves(a);

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
	show_2_halves(a);

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
	show_2_halves(a);

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
	show_2_halves(a);

	if (HC_HALF_BITS == 16) {
		assert(a[1] == 0xfffe && a[0] == 0x0003);
	} else {
		HC_FATAL("not implemented");
	}
}

#if (HC_HALF_BYTES == 2)
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

	hcns(s_alloc)(s, s->len + n->len * HC_HALF_BYTES);

	for (i=ndigits-1; i>=0; i--) {
		char t[4];
		hcns(h) d = n->d[i];
		t[0] = HC_HEX_DIGIT(d >> 12);
		t[1] = HC_HEX_DIGIT(d >> 8);
		t[2] = HC_HEX_DIGIT(d >> 4);
		t[3] = HC_HEX_DIGIT(d);
		hcns(s_catn)(s, t, 4);
	}
}

#else
#error not implemented
#endif

#define HC_SAFE_C_STR(s) do { hcns(s_catn)(s, "\0", 1); --(s)->len; } while (0)

static void test_hex_in_out()
{
	char hexstr[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	printf("sizeof(hexstr): %i\n", (int)sizeof(hexstr));

	if (1) {
		struct hcns(n) a = HC_NULL_N;
		struct hcns(s) s = HC_NULL_S;
		unsigned char valstr[sizeof(hexstr) / 2];
	
		HC_GET_HEX(valstr, sizeof(valstr), hexstr);
		printf("%02x .. %02x\n", valstr[0], valstr[sizeof(valstr) - 1]);

		hcns(n_load_be1)(&a, valstr, sizeof(valstr));

		printf("[0] = " HC_FMT_H " [1] = " HC_FMT_H " [%i-2] = " HC_FMT_H " [%i-1] = " HC_FMT_H "\n",
			a.d[0], a.d[1], a.len, a.d[a.len-2], a.len, a.d[a.len-1]);

		s.len = 0;
		hcns(n_as_hex)(&a, &s);
		HC_SAFE_C_STR(&s);

		assert(hcns(sdiffn)(s.s, hexstr, sizeof(hexstr)) == 0);

		puts(s.s);
	}

	if (1) {
		struct hcns(n) a = HC_NULL_N;
		struct hcns(s) s = HC_NULL_S;

		hcns(n_load_hex)(&a, hexstr, sizeof(hexstr));
		printf("[0] = " HC_FMT_H " [1] = " HC_FMT_H " [%i-2] = " HC_FMT_H " [%i-1] = " HC_FMT_H "\n",
			a.d[0], a.d[1], a.len, a.d[a.len-2], a.len, a.d[a.len-1]);

		s.len = 0;
		hcns(n_as_hex)(&a, &s);
		HC_SAFE_C_STR(&s);

		puts(s.s);

		assert(hcns(sdiffn)(s.s, hexstr, sizeof(hexstr)) == 0);
	}
}

static void print_n(char *prefix, struct hcns(n) *n, char *suffix)
{
	struct hcns(s) s = HC_NULL_S;

	hcns(s_catz)(&s, prefix);
	hcns(n_as_hex)(n, &s);
	hcns(s_catz)(&s, suffix);
	HC_SAFE_C_STR(&s);

	printf("%s", s.s);
}

static void n_hex_cmp(struct hcns(n) *n, char *hex)
{
	struct hcns(s) s = HC_NULL_S;

	hcns(n_as_hex)(n, &s);
	HC_SAFE_C_STR(&s);

	assert(hcns(sdiffn)(s.s, hex, s.len) == 0);
}

static void test_mul1()
{
	char a_hex[40] = "da39a3ee5e6b4b0d3255bfef95601890afd80709"; /* sha1 of no data */
	struct hcns(n) a = HC_NULL_N;
	struct hcns(n) r = HC_NULL_N;

	hcns(n_load_hex)(&a, a_hex, sizeof(a_hex));

	hcns(n_alloc)(&r, a.len + 1);

	r.len = D_mul(a.d, a.len, HC_H(0), r.d);

	print_n("r = ", &r, "\n");
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
