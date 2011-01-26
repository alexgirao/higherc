#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "higherc/higherc.h"
#include "higherc/bytewise.h"
#include "higherc/byte.h"

int main(int argc, char **argv)
{
	char a[] = "123alpha";
	char b[] = "bravo123";

	/* shift left
	 */
	hcns(bcopyl)(a, 5, a + 3);

	/* shift right
	 */

	hcns(bcopyr)(b + 3, 5, b);

	assert(hcns(bdiff)(a, 8, "alphapha") == 0);
	assert(hcns(bdiff)(b, 8, "brabravo") == 0);

	printf("[%s]\n", a);
	printf("[%s]\n", b);

	return 0;
}
