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
