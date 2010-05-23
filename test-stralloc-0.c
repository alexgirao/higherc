#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/str.h"
#include "higherc/s.h"

static void print_s(char *prefix, HC_ST_S *s, char *suffix)
{
	if (prefix) printf("%s", prefix);
	fwrite(s->s, s->len, 1, stdout);
	if (suffix) printf("%s", suffix);
}

int main(int argc, char **argv)
{
	HC_DEF_S(s);

	hcns(s_catz)(s, "hello world of possibilities!");
	HC_SAFE_CSTR(s);

	fprintf(stdout, "[%i][%i][%s]\n", s->len, s->a, s->s);

	s->len = 0;  /* truncate string */

	hcns(s_catn)(s, "hi ", 3);
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_copyz)(s, "folks!\n");
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_upper)(s);
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_lower)(s);
	fwrite(s->s, s->len, 1, stdout);

	hcns(s_copyz)(s, "aa");

	assert(hcns(sdiff)("aa", "aa") == 0);
	assert(hcns(s_diffz)(s, "aa") == 0);
	assert(hcns(s_diffn)(s, "aa", hcns(slen("aa"))) == 0);

	assert(hcns(sdiff)("aa", "aaz") < 0);
	assert(hcns(s_diffz)(s, "aaz") < 0);
	assert(hcns(s_diffn)(s, "aaz", hcns(slen("aaz"))) < 0);

	assert(hcns(sdiff)("aa", "a0z") > 0);
	assert(hcns(s_diffz)(s, "a0z") > 0);
	assert(hcns(s_diffn)(s, "a0z", hcns(slen("a0z"))) > 0);

	assert(hcns(sdiff)("aa", "0") > 0);
	assert(hcns(s_diffz)(s, "0") > 0);
	assert(hcns(s_diffn)(s, "0", hcns(slen("0"))) > 0);

	assert(hcns(sdiff)("aa", "0zz") > 0);
	assert(hcns(s_diffz)(s, "0zz") > 0);
	assert(hcns(s_diffn)(s, "0zz", hcns(slen("0zz"))) > 0);

	assert(hcns(sdiff)("aa", "a") > 0);
	assert(hcns(s_diffz)(s, "a") > 0);
	assert(hcns(s_diffn)(s, "a", hcns(slen("a"))) > 0);

	assert(hcns(sdiff)("aa", "a0") > 0);
	assert(hcns(s_diffz)(s, "a0") > 0);
	assert(hcns(s_diffn)(s, "a0", hcns(slen("a0"))) > 0);

	hcns(s_free)(s);

	/*
	 */

	hcns(s_cat_u4_dec)(s, 0xffffffff);
	assert(hcns(s_diffz)(s, "4294967295") == 0);
	print_s("[", s, "]\n");

	s->len = 0;
	hcns(s_cat_i4_dec)(s, (hcns(i4))0xffffffff);
	assert(hcns(s_diffz)(s, "-1") == 0);
	print_s("[", s, "]\n");

	hcns(s_free)(s);

	/*
	 */

	hcns(s_free)(s);

	return 0;
}
