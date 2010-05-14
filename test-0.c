#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"
#include "higherc/rand.h"

int main(int argc, char **argv)
{
	char c = 0xef;

	printf("1: %x\n", (int)c);              // two's complement aware
	printf("2: %x\n", (int)(char)c);        // likewise
	printf("3: %x\n", (unsigned int)c);     // likewise (sign-bit is carried on type change)
	printf("4: %x\n", (unsigned int)(unsigned char)c);        // bitwise ;-)
	printf("5: %x\n", (int)(unsigned char)c);        // bitwise ;-)
	printf("6: %x\n", (int)((char)c & 0xff));        // strip sign-bit
	printf("7: %x\n", (int)(c & 0xff));              // likewise

	return 0;
}
