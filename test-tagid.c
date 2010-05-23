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

	HC_SAFE_CSTR(tagid->A);
	printf("tagid.A: %s\n", tagid->A->s);
	printf("tagid.B: %i\n", tagid->B);
	printf("tagid.C: %i\n", tagid->C);
	printf("tagid.D: %.8x\n", tagid->D);

	{
		HC_DEF_S(E);
		hcns(n_be1_as_base36)(E, tagid->E, sizeof(tagid->E));
		HC_SAFE_CSTR(E);
		printf("tagid.E: %s\n", E->s);
		hcns(s_free)(E);
	}

        /* cleanup
	 */

	hcns(tag_free)(h);
	hcns(tagid_free)(tagid);

	return 0;
}
