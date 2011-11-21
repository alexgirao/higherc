/*
 * Copyright (C) 2010 Alexandre Girao <alexgirao@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>  /* /usr/include/asm-generic/errno.h */
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/bytewise.h"

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
		errno = 0; /* discard error */
	}
	return r;
}

static int safe_write(int fd, char *buf, int count)
{
	ssize_t r = write(fd, buf, count);
	if (r < 0) {
		assert(r == -1);  // just to detect weird systems/weird bugs
		fprintf(stderr, "write() = %li, errno: %i\n", (long int) r, errno);
		fflush(stderr);
		perror("write()");
		exit(1);
	}
	if (r == 0 && errno) {
		if (errno != ENOTTY /* Not a typewriter */) {
			/* interesting values to check: EWOULDBLOCK
			 * and EAGAIN for non-blocking descriptors */
			fprintf(stderr, "write() = 0 and errno set, errno: %i\n", errno);
			fflush(stderr);
			perror("write()");
			exit(1);
		}
		errno = 0; /* discard error */
	}
	return r;
}

int hcns(read_exact)(int fd, void *buf, int len)
{
	int i, got = 0;

	do {
		i = safe_read(fd, buf + got, len - got);
		assert(i > 0);
		got += i;
	} while (got < len);

	assert(got == len);

	return len;
}

int hcns(write_exact)(int fd, void *buf, int len)
{
	int i, wrote = 0;

	do {
		i = safe_write(fd, buf + wrote, len - wrote);
		assert(i > 0);
		wrote += i;
	} while (wrote < len);

	assert(wrote == len);

	return len;
}

int hcns(readfd)(int fd, void *buf, int bufsz, int (*doit)(const char *buf, int len, hcns(bool) eof))
{
	int total = 0;
	int len = 0;

	assert(bufsz > 0);
	assert(errno == 0 || errno == ENOTTY);

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
				/* no data to process
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

		if (n == 0 && len == bufsz) {
			HC_FATAL("buffer too small");
		}

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
				hcns(b_copyl)(buf, len - n, buf + n);
			}
			len -= n;
			total += n;
		}
    	}

	return total;
}

hcns(u4) hcns(readfd_be4)(int fd)
{
	unsigned int val;
	int len;

	assert(errno == 0 || errno == ENOTTY);

	len = hcns(read_exact)(fd, &val, sizeof(val));
	assert(len == 4);

	return HC_GET_BE4(&val);
}
