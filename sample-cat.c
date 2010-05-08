/* test with:
 *
 *    echo -n | ./sample-cat | wc -c
 *
 *    ./sample-cat < README 2>/dev/null | sha1sum
 *    cat < README | sha1sum
 *
 *    bzcat /usr/src/linux-source-2.6.28.tar.bz2 | dd bs=4096 count=1024 2>/dev/null | ./sample-cat | sha1sum
 *    bzcat /usr/src/linux-source-2.6.28.tar.bz2 | dd bs=4096 count=1024 2>/dev/null | sha1sum
 *    bzcat /usr/src/linux-source-2.6.28.tar.bz2 | dd bs=4096 count=1024 2>/dev/null | wc -c && echo $((4096*1024))
 *
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

static int aaa = 0;

/* processing function, return amount of successfully processed data
 */
static int doit(const char *buf, int len, hcns(bool) eof) {
	int i;

	if (eof) {
		/* we are at EOF, MUST consume ALL data
		 */
		i = 0;
		do {
			int n = write(1 /* STDOUT */, buf + i, 10 < len - i ? 10 : len - i);
			if (n <= 0) {
				/* write must block until it can write
				 * something successfully
				 */
				perror("write");
				exit(1);
			}
			i += n;
		} while (i < len);
	} else {
		if (aaa++ == 3) {
			/* just a test, tells that nothing was
			 * processed, possible only if we are not at
			 * EOF
			 */
			return 0;
		}
		i = write(1 /* STDOUT */, buf, len);
	}

	fprintf(stderr, "processed %i bytes out of %i, %i left out, eof? %i\n", i, len, len - i, eof);
	return i;
}

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

int main(int argc, char **argv)
{
	char buf[0x10000];
	int total = 0;
	int len = 0;

	for (;;) {
		int n;
		hcns(bool) eof = 0;

		if (len < sizeof(buf)) {
			/* still data to read and room to feed
			 * processing function
			 */
			do {
				/* read until buffer is full or EOF
				 */
				n = safe_read(0 /* STDIN */, buf + len, sizeof(buf) - len);
				eof = n == 0;
				if (eof) {
					/* EOF */
					break;
				}
				len += n;
			} while (len < sizeof(buf));

			assert(len <= sizeof(buf));

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
				 * buffer
				 */
				hcns(bcopyr)(buf, len - n, buf + n);
			}
			len -= n;
			total += n;
		}
    	}

	fprintf(stderr, "total bytes read: %i\n", total);

	return 0;
}
