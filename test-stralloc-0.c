#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/str.h"
#include "higherc/stralloc.h"

int main(int argc, char **argv)
{
	struct hcns(s) sa0 = HC_NULL_S;

	hcns(s_catz)(&sa0, "hello world of possibilities!");
	hcns(s_catn)(&sa0, "", 1); /* append '\0' */

	fprintf(stdout, "[%i][%i][%s]\n", sa0.len, sa0.a, sa0.s);

	sa0.len = 0;  /* truncate string */

	hcns(s_catn)(&sa0, "hi ", 3);
	fwrite(sa0.s, sa0.len, 1, stdout);

	hcns(s_copyz)(&sa0, "folks!\n");
	fwrite(sa0.s, sa0.len, 1, stdout);

	hcns(s_upper)(&sa0);
	fwrite(sa0.s, sa0.len, 1, stdout);

	hcns(s_lower)(&sa0);
	fwrite(sa0.s, sa0.len, 1, stdout);

	hcns(s_copyz)(&sa0, "aa");
	assert(hcns(s_sdiff)(&sa0, "aa") == 0);

	assert(hcns(sdiff)("aa", "aaz") < 0);
	assert(hcns(s_sdiff)(&sa0, "aaz") < 0);
	assert(hcns(sdiff)("aa", "a0z") > 0);
	assert(hcns(s_sdiff)(&sa0, "a0z") > 0);

	hcns(s_free)(&sa0);

	fflush(stdout);

	return 0;
}
