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
#include <unistd.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"

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

int main(int argc, char **argv)
{
	int bufsz = 1024 * 1024;
	void *buf;

	HC_ALLOC(buf, bufsz);

	fprintf(stderr, "total bytes read: %i\n", hcns(readfd)(0 /* STDIN */, buf, bufsz, doit));

	HC_FREE(buf);

	return 0;
}
