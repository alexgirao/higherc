#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
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

	hcns(s_copyz)(&sa0, "folks!");
	fwrite(sa0.s, sa0.len, 1, stdout);

	fwrite("\n", 1, 1, stdout);

	hcns(s_free)(&sa0);

	fflush(stdout);

	return 0;
}
