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
#include "higherc/fatal.h"
#include "higherc/n.h"

#define DEBUG_N(n) do {							\
		printf(__FILE__ ":%i: ", __LINE__);			\
		print_n(#n " [", n, "]");				\
		printf(" (d: %p, len: %i, a: %i)\n", (n)->d, (n)->len, (n)->a); \
		fflush(stdout);						\
	} while (0)

static void print_n(char *prefix, struct hcns(n) *n, char *suffix)
{
	struct hcns(s) s = HC_NULL_S;

	hcns(s_catz)(&s, prefix);
	hcns(n_as_hex)(n, &s);
	hcns(s_catz)(&s, suffix);

	HC_SAFE_CSTR(&s);

	printf("%s", s.s);

	hcns(s_free)(&s);
}

#define N struct hcns(n)
#define S struct hcns(s)
#define DEF_N(sym) N sym[1] = {HC_NULL_N}
#define DEF_S(sym) S sym[1] = {HC_NULL_S}

#define HC_BASE36_DIGIT(v) (v >= 0 && v < 36 ? "0123456789abcdefghijklmnopqrstuvwxyz"[v] : '?')
#define HC_IS_ZERO(n) ((n)->len == 0 || ((n)->len == 1 && (n)->d[0] == HC_H(0)))

#define B36_2 1296  /* pow(36, 2) */
#define B36_3 46656  /* pow(36, 3) */

#if HC_HALF_BITS == 32
#define B36_4 1679616
#define B36_5 60466176
#define B36_6 2176782336 /* 6 digits after each 2176782336 division */
#endif

void hcns(n_as_base36)(N *n, S *s)
{
	DEF_N(q);
	hcns(h) r = HC_H(0);
	int s_len0 = s->len;

	if (HC_IS_ZERO(n)) {
		hcns(s_catn)(s, "0", 1);
		return;
	}

	hcns(n_alloc)(q, n->len);
	HC_MOVE_DIGITS(q->d, n->d, n->len);
	q->len = n->len;

	DEBUG_N(q);

	while (!HC_IS_ZERO(q)) {
		q->len = D_div(q->d, q->len, HC_H(36), q->d, &r);

		DEBUG_N(q);

		hcns(s_alloc)(s, s->len + 1);
		s->s[s->len++] = HC_BASE36_DIGIT(r);
	}

	hcns(brev)(s->s + s_len0, s->len - s_len0);
}

void hcns(n_as_base36_2)(N *n, S *s)
{
	DEF_N(q);
	hcns(h) r = HC_H(0);
	int s_len0 = s->len;

	if (HC_IS_ZERO(n)) {
		hcns(s_catn)(s, "0", 1);
		return;
	}

	hcns(n_alloc)(q, n->len);
	HC_MOVE_DIGITS(q->d, n->d, n->len);
	q->len = n->len;

	DEBUG_N(q);

	while (!HC_IS_ZERO(q)) {
		q->len = D_div(q->d, q->len, B36_3, q->d, &r);

		DEBUG_N(q);

		hcns(s_alloc)(s, s->len + 3);

		s->s[s->len++] = HC_BASE36_DIGIT(r % 36);
		s->s[s->len++] = HC_BASE36_DIGIT((r / 36) % 36);
		s->s[s->len++] = HC_BASE36_DIGIT((r / B36_2) % 36);
	}

	while (s->s[s->len - 1] == '0') s->len--;

	hcns(brev)(s->s + s_len0, s->len - s_len0);
}

int main(int argc, char **argv)
{
	char *a_hex = "15bc3c38c53340b1e98c3be3671a378276b7613c";
	char *a_b36 = "2jeg2ovjrtl0hfk79vsjjfva7prvj24";
	int a_hex_len = hcns(slen)(a_hex);
	DEF_N(a);
	DEF_S(s);

	hcns(n_load_hex)(a, a_hex, a_hex_len);

	/*
	 */

	hcns(n_as_base36)(a, s);
	HC_SAFE_CSTR(s);

	assert(hcns(s_sdiff)(s, a_b36) == 0);

	puts(s->s);

	/*
	 */

	s->len = 0;
	hcns(n_as_base36_2)(a, s);
	HC_SAFE_CSTR(s);

	assert(hcns(s_sdiff)(s, a_b36) == 0);

	puts(s->s);

	/*
	 */

	hcns(n_set_u4)(a, 0);

	s->len = 0;
	hcns(n_as_base36)(a, s);
	HC_SAFE_CSTR(s);
	assert(hcns(s_sdiff)(s, "0") == 0);

	puts(s->s);

	s->len = 0;
	hcns(n_as_base36_2)(a, s);
	HC_SAFE_CSTR(s);
	assert(hcns(s_sdiff)(s, "0") == 0);

	puts(s->s);

	return 0;
}
