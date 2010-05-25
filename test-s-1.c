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

	/*
	 */

	hcns(s_catz)(s, "alpha...bravo");

	hcns(s_shiftr)(s, 5, 8, 3, '0');
	assert(hcns(s_diffz)(s, "alpha000bravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	hcns(s_shiftr)(s, 5, 9, 3, '1');
	assert(hcns(s_diffz)(s, "alpha1110ravo") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	hcns(s_shiftr)(s, 5, -1, 3, '2');
	assert(hcns(s_diffz)(s, "alpha2221110o") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	hcns(s_shiftr)(s, 5, s->len, 3, '3');
	assert(hcns(s_diffz)(s, "alpha33322211") == 0 && s->len == 13);
	print_s("[", s, "]\n");

	hcns(s_shiftr)(s, 5, s->len + 1, 3, '4');
	assert(hcns(s_diffz)(s, "alpha444333222") == 0 && s->len == 14);
	print_s("[", s, "]\n");

	hcns(s_shiftr)(s, 5, s->len + 3, 3, '5');
	assert(hcns(s_diffz)(s, "alpha555444333222") == 0 && s->len == 17);
	print_s("[", s, "]\n");

	/*
	 */

	hcns(s_free)(s);

	return 0;
}
