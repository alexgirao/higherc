#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>  /* /usr/include/asm-generic/errno.h */
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/readfd.h"

/* safe_read: when you must fail, fail noisily and as soon as possible
 */
static int safe_read(int fd, char *buf, int count)
{
	ssize_t r = read(fd, buf, count);
	if (r < 0) {
		assert(r == -1);  // just to detect weird systems/weird bugs
		fprintf(stderr, "read() = %li, errno: %i\n", (long int) r, errno);
		fflush(stderr);
		perror("read()");
		exit(1);
	}
	if (r == 0 && errno) {
		if (errno != ENOTTY /* Not a typewriter */) {
			/* interesting values to check: EWOULDBLOCK
			 * and EAGAIN for non-blocking descriptors */
			fprintf(stderr, "read() = 0 and errno set, errno: %i\n", errno);
			fflush(stderr);
			perror("read()");
			exit(1);
		}
	}
	return r;
}

int hcns(readfd)(int fd, void *buf, int bufsz, int (*doit)(const char *buf, int len, hcns(bool) eof))
{
	int total = 0;
	int len = 0;

	assert(bufsz > 0);

	for (;;) {
		int n;
		hcns(bool) eof = 0;

		if (len < bufsz) {
			/* still data to read and room to feed
			 * processing function
			 */
			do {
				/* read until buffer is full or EOF
				 */
				n = safe_read(fd, buf + len, bufsz - len);
				eof = n == 0;
				if (eof) {
					/* EOF */
					break;
				}
				len += n;
			} while (len < bufsz);

			assert(len <= bufsz);

			if (len == 0) {
				/* no data at STDIN
				 */
				break;
			}
		}

		/* process data
		 */
		n = doit(buf, len, eof);

		assert(n >= 0);
		assert(n <= len);
		assert(!eof || n == len); /* processing function MUST
					   * consume ALL data upon
					   * EOF */

		if (eof) {
			total += len;
			break;
		} else if (n > 0) {
			/* data was consumed by processing function
			 */
			if (len - n > 0) {
				/* copy unprocessed data to start of
				 * buffer (shift left)
				 */
				hcns(bcopyl)(buf, len - n, buf + n);
			}
			len -= n;
			total += n;
		}
    	}

	return total;
}
