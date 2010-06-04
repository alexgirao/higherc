#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"
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
	int i;
	HC_DEF_S(s);
	HC_DEF_S(s_repr);

	/*
	 */

	for (i=0; i<=100; i++) {
		hcns(s_catc)(s, i);
	}
	hcns(s_repr)(s_repr, s);

	print_s("[", s_repr, "]\n");

	/*
	 */

	s_repr->len = 0;
	hcns(s_reprz)(s_repr, "\x1\x2\x3");

	print_s("[", s_repr, "]\n");

	/*
	 */

	hcns(s_free)(s_repr);
	hcns(s_free)(s);

	return 0;
}
