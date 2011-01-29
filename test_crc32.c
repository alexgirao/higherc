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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "higherc/higherc.h"
#include "higherc/crc32.h"

/* crc32 tested with jacksum (java.util.zip.CRC32) and erlang:crc32/1
 */

static char test0[] = "hello world of possibilities!!!";
static char test1[] = "#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghij$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijk";

int main(int argc, char **argv)
{
	assert(hcns(crc32)(0, test0, sizeof test0 - 1) == -1625496575);
	assert(hcns(crc32)(0, test0, sizeof test0 - 1) == 0x9f1ce401 /* -1625496575 */);
	assert(hcns(crc32)(0, test1, sizeof test1 - 1) == 1690350392);
	puts("ok");
	return 0;
}
