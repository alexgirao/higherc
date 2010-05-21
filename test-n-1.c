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

int main(int argc, char **argv)
{
	char *a_hex = "15bc3c38c53340b1e98c3be3671a378276b7613c";
	char *a_dec = "124086592339716874826150129472345739134649196860";
	char *a_b36 = "2jeg2ovjrtl0hfk79vsjjfva7prvj24";
	int a_hex_len = hcns(slen)(a_hex);
	HC_DEF_N(a);
	HC_DEF_S(s);

	if (0) {
		struct hcns(n) _unused = HC_NULL_N;
		DEBUG_N(&_unused);
	}

	/* load a_hex
	 */

	hcns(n_load_hex)(a, a_hex, a_hex_len);

	/* base36
	 */

	s->len = 0; /* truncate s */
	hcns(n_as_base36)(a, s);
	HC_SAFE_CSTR(s);
	assert(hcns(s_sdiff)(s, a_b36) == 0);

	/* decimal
	 */

	s->len = 0; /* truncate s */
	hcns(n_as_dec)(a, s);
	HC_SAFE_CSTR(s);
	assert(hcns(s_sdiff)(s, a_dec) == 0);

	/* load 0
	 */

	hcns(n_set_u4)(a, 0);

	/* base36
	 */

	s->len = 0; /* truncate s */
	hcns(n_as_base36)(a, s);
	HC_SAFE_CSTR(s);
	assert(hcns(s_sdiff)(s, "0") == 0);

	/* decimal
	 */

	s->len = 0; /* truncate s */
	hcns(n_as_dec)(a, s);
	HC_SAFE_CSTR(s);
	assert(hcns(s_sdiff)(s, "0") == 0);

	/*
	 */

	puts("ok");

	return 0;
}
