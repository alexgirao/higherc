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
#include "higherc/alloc.h"

#define VALID_LINE 1
#define SKIP_LINE 2

/* return amount of successfully processed buffer
 */
static int doit(const char *buf, int len, hcns(bool) eof)
{
	int n, c;
	const char *start = buf;
	int state = VALID_LINE;
	int line_end = 0;  /* be it valid or not */

	for (n=0;n<len;) {
		c = buf[n++];

		if (state == VALID_LINE) {
			if (c == 10 /* LF */) {
				assert(start != NULL);
				line_end = n;
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
				line_end = n;
				state = VALID_LINE;
				start = buf + n;
			}
			continue;
		}
		HC_FATAL("exhausted");
	}

	if (eof) {
		if (start) {
			fwrite(start, (buf + n) - start, 1, stdout);
		}
		return len;
	}

	/* do not consume partial lines
	 */
	return line_end;
}

int main(int argc, char **argv)
{
	int bufsz = 1024 * 1024;
	void *buf;

	HC_ALLOC(buf, bufsz);

	fprintf(stderr, "total bytes read: %i\n", hcns(readfd)(0 /* STDIN */, buf, bufsz, doit));

	HC_FREE(buf);

	return 0;
}
