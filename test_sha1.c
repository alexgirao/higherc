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

/*

find -type f | xargs -n1 -Iiii sh -c 'echo -n iii " "; sha1sum < iii | cut -d" " -f1' > ../a
find -type f | xargs -n1 -Iiii sh -c 'echo -n iii " "; ./test_sha1 < iii' > ../b

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"
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

	assert(hcns(b_diff)(sha1_1str, 41, sha1_2str) == 0);

	puts(sha1_1str);

	return 0;
}
