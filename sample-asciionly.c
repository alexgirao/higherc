#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>  /* /usr/include/asm-generic/errno.h */
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/bytewise.h"
#include "higherc/s.h"
#include "higherc/tagid.h"
#include "higherc/list.h"
#include "higherc/pbuf.h"
#include "higherc/readfd.h"

#define VALID_LINE 1
#define SKIP_LINE 2

/* return amount of successfully processed buffer
 */
static int doit(const char *buf, int len, hcns(bool) eof)
{
	int n, c;
	const char *start = buf;
	int state = VALID_LINE;

	for (n=0;n<len;) {
		c = buf[n++];

		if (state == VALID_LINE) {
			if (c == 10 /* LF */) {
				assert(start != NULL);
				fwrite(start, (buf + n) - start, 1, stdout);
				start = buf + n;
			} else if (!HC_ISASCII(c)) {
				state = SKIP_LINE;
				start = NULL;
				continue;
			}
			continue;
		} else if (state == SKIP_LINE) {
			if (c == 10 /* LF */) {
				assert(start == NULL);
				state = VALID_LINE;
				start = buf + n;
			}
			continue;
		}
		HC_FATAL("exhausted");
	}

	if (start) {
		fwrite(start, (buf + n) - start, 1, stdout);
	}

	return len;
}

int main(int argc, char **argv)
{
	fprintf(stderr, "total bytes read: %i\n", hcns(readfd)(0 /* STDIN */, doit));
	return 0;
}
