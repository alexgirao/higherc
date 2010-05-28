/*

find -type f | xargs -n1 -Iiii sh -c 'echo -n iii " "; sha1sum < iii | cut -d" " -f1' > ../a
find -type f | xargs -n1 -Iiii sh -c 'echo -n iii " "; ./test-sha1 < iii' > ../b

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"
#include "higherc/readfd.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"
#include "higherc/sha1.h"

int main()
{
	int bufsz;
	void *buf;
	struct hcns(sha1) ctx;
	unsigned char sha1_1[20];
	unsigned char sha1_2[20];
	char sha1_1str[41];
	char sha1_2str[41];

   	/* processing function, return amount of successfully
	 * processed data
	 */
	int doit(const char *buf, int len, hcns(bool) eof) {
		hcns(sha1_update)(&ctx, buf, len);
    		return len;
	}

	bufsz = 1024 * 1024;
	HC_ALLOC(buf, bufsz);

	hcns(sha1_init)(&ctx);
	int i = hcns(readfd)(0 /* STDIN */, buf, bufsz, doit);
	hcns(sha1_final)(&ctx, sha1_1);

	HC_FREE(buf);

	fprintf(stderr, "total bytes read: %i\n", i);

	memset(sha1_1str, 'x', sizeof(sha1_1str));
	memset(sha1_2, 'y', sizeof(sha1_2));
	memset(sha1_2str, 'z', sizeof(sha1_2str));

	HC_PUT_HEX(sha1_1str, 20, sha1_1);
	sha1_1str[40] = '\0';

	HC_GET_HEX(sha1_2, 20, sha1_1str);

	HC_PUT_HEX(sha1_2str, 20, sha1_2);
	sha1_2str[40] = '\0';

	assert(hcns(bdiff)(sha1_1str, 41, sha1_2str) == 0);

	puts(sha1_1str);

	return 0;
}
