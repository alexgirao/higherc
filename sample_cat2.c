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

/* line oriented processing
 */

/* test with:
 *
 *    echo -n | ./sample_cat2 | wc -c
 *
 *    ./sample_cat2 < README 2>/dev/null | sha1sum
 *    cat < README | sha1sum
 *
 *    bzcat /usr/src/linux-source-2.6.28.tar.bz2 | dd bs=4096 count=1024 2>/dev/null | ./sample_cat2 | sha1sum
 *    bzcat /usr/src/linux-source-2.6.28.tar.bz2 | dd bs=4096 count=1024 2>/dev/null | sha1sum
 *    bzcat /usr/src/linux-source-2.6.28.tar.bz2 | dd bs=4096 count=1024 2>/dev/null | wc -c && echo $((4096*1024))
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"

static const char *get_line(const char *buf, int buflen, int *llenp)
{
	const char *cp = buf;
	while (*cp != '\n' && (cp - buf) < buflen) {
		cp++;
	}
	*llenp = cp - buf;
	return buf;
}

/* processing function, return amount of successfully processed data
 */
static int doit(const char *buf, int len, bool eof)
{
	int i;
	const char *line;
	int line_len;

	i = 0;
	while (i < len && (line = get_line(buf+i, len-i, &line_len)) != NULL) {
		assert(i + line_len <= len);
		if (len-i == line_len) {
			/* all data processed, no '\n' was found on
			 * last line */
			if (eof) {
				/* spit last bytes */
				fwrite(line, len-i, 1, stdout);
				i = len;
				break;
			} else {
				/* need more data to complete a line */
				if (i == 0) {
					/* buffer too small, line too long */
					HC_FATAL("line too long, max allowed is %i bytes\n", line_len - 1);
				}
				break;
			}
		} else {
			/* got a line */
			fwrite(line, line_len, 1, stdout);
			fputs("\n", stdout);
			i += line_len + 1; /* +1 so we skip '\n' char */
		}
	}

	assert(i <= len);

	fprintf(stderr, "processed %i bytes out of %i, %i left out, eof? %i\n", i, len, len - i, eof);
	return i;
}

int main(int argc, char **argv)
{
	int bufsz = 0xffff;
	void *buf;

	HC_ALLOC(buf, bufsz);

	fprintf(stderr, "total bytes read: %i\n", readfd(0 /* STDIN */, buf, bufsz, doit));

	HC_FREE(buf);

	return 0;
}
