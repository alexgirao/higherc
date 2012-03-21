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
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"

void test0()
{
	char a[] = "123alpha";
	char b[] = "bravo123";

	/* shift left
	 */
	b_copyl(a, 5, a + 3);

	/* shift right
	 */

	b_copyr(b + 3, 5, b);

	assert(b_diff(a, 8, "alphapha") == 0);
	assert(b_diff(b, 8, "brabravo") == 0);

	printf("[%s]\n", a);
	printf("[%s]\n", b);
}

void test1()
{
	char a[] = "123alpha";
	char b[] = "bravo123";

	/* shift left
	 */
	b_copy(a, 5, a + 3);

	/* shift right
	 */

	b_copy(b + 3, 5, b);

	assert(b_diff(a, 8, "alphapha") == 0);
	assert(b_diff(b, 8, "brabravo") == 0);

	printf("[%s]\n", a);
	printf("[%s]\n", b);
}

int main(int argc, char **argv)
{
	test0();
	test1();
	return 0;
}
