/* test with:
 *
 *    dd if=/dev/zero bs=1MB count=4 2>/dev/null | ./sample-cat 2>/dev/null | sha1sum
 *    dd if=/dev/zero bs=1MB count=4 2>/dev/null | cat | sha1sum
 *    dd if=/dev/zero bs=1MB count=4 2>/dev/null | sha1sum
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>  /* /usr/include/asm-generic/errno.h */
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/list.h"
#include "higherc/pbuf.h"

/* return amount of successfully processed buffer
 */
static int doit(const char *buf, int len, hcns(bool) eof) {
	int i = write(1 /* STDOUT */, buf, len);
	fprintf(stderr, "processed %i bytes out of %i, %i left out\n", i, len, len - i);
	return i;
}

static int read_check(int fd, char *buf, int count)
{
	ssize_t r = read(fd, buf, count);
	if (r < 0) {
		assert(r == -1);  // just to detect weird systems/weird bugs
		fprintf(stderr, "read() = %li, errno: %i\n", r, errno);
		fflush(stderr);
		perror("read()");
		exit(1);
	}
	if (r == 0 && errno) {
		if (errno != ENOTTY /* Not a typewriter */) {
			/* interesting values to check: EWOULDBLOCK EAGAIN for
			 * non-blocking descriptors */
			fprintf(stderr, "read() = 0 and errno set, errno: %i\n", errno);
			fflush(stderr);
			perror("read()");
			exit(1);
		}
	}
	return r;
}

int main(int argc, char **argv)
{
	char buf[0x10000];
	int total = 0;
	int len = 0;
	hcns(bool) eof = 0;

	for (;;) {
		if (!eof) {
			/* still data to read
			 */
			do {
				/* read until buffer is full or EOF
				 */
				int n = read_check(0 /* STDIN */, buf + len, sizeof(buf) - len);
				eof = n == 0;
				if (eof) {
					/* EOF */
					break;
				}
				len += n;
			} while (len < sizeof(buf));

			assert(len <= sizeof(buf));
		}

		if (len) {
			/* process data
			 */
			int n = doit(buf, len, eof);
			assert(n >= 0 && n <= len);

			total += n;

			if (len - n > 0) {
				hcns(bcopyr)(buf, len - n, buf + n);
			}
			len -= n;
		} else {
			/* nothing more to process */
			break;
		}
    	}

	fprintf(stderr, "total bytes read: %i\n", total);

	return 0;
}
