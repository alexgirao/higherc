#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/readfd.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"
#include "higherc/alloc.h"
#include "higherc/s.h"
#include "higherc/n.h"
#include "higherc/sha1.h"
#include "higherc/crc32.h"
#include "higherc/tagid.h"

/* see ~/projects/tagdb/README2
 */

static void print_s(char *prefix, HC_ST_S *s, char *suffix)
{
	if (prefix) printf("%s", prefix);
	fwrite(s->s, s->len, 1, stdout);
	if (suffix) printf("%s", suffix);
}

void s_shiftr(HC_ST_S *s, int start, int n, char pad)
{
	int i;

	assert(start >= 0);
	assert(start <= s->len);

	printf("-------------------------------------------------------------------------------- %i\n", s->len - start);

	hcns(s_alloc)(s, s->len + n);
	hcns(bcopyr)(s->s + start + n, s->len - start, s->s + start);

	s->len += n;
	for (i=0;i<n;i++) {
		s->s[start+i] = pad;
	}
}

void print_tagid(HC_ST_TAGID *tagid)
{
	HC_DEF_S(E);

	HC_SAFE_CSTR(tagid->A);
	printf("tagid.A: %s\n", tagid->A->s);
	printf("tagid.B: %i\n", tagid->B);
	printf("tagid.C: %i\n", tagid->C);
	printf("tagid.D: %.8x\n", tagid->D);

	hcns(n_be1_as_base36)(E, tagid->E, sizeof(tagid->E));
	HC_SAFE_CSTR(E);
	printf("tagid.E: %s\n", E->s);

	HC_DEF_S(B);

	hcns(s_catz)(B, "alpha");
	//hcns(s_catn)(B, "\0", 1);
	s_shiftr(B, B->len, 30, '0');
	print_s("B: [", B, "]\n");

	return;


	B->len = 3;
	int b_len = hcns(s_cat_u4_hex)(B, tagid->B);
	assert(b_len <= 2);
	s_shiftr(B, 3, 2 - b_len, '0');

	printf("B->len: %i\n", B->len);
	print_s("B: [", B, "]\n");
	hcns(s_free)(B);
       
	hcns(s_free)(E);
}

int main(int argc, char **argv)
{
	int i;
	HC_ST_TAG *h = NULL;
	HC_DEF_TAGID(tagid);

	for (i=1; i<argc; i++) {
		h = hcns(tag_new)(h, argv[i]);
		if (h == NULL) {
			HC_FATAL("invalid tag at arg %i", i);
		}
	}

	if (h == NULL) {
		/* no arguments
		 */
		printf("no arguments supplied\n");
		exit(0);
	}

	hcns(tagid_set)(tagid, h);
	print_tagid(tagid);

        /* cleanup
	 */

	hcns(tag_free)(h);
	hcns(tagid_free)(tagid);

	return 0;
}
