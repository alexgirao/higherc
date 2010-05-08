#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"

int main(int argc, char **argv)
{
	char buf[1024];
	ssize_t rsz;
	char c = 0xef;

	printf("1: %x\n", (int)c);              // two's complement aware
	printf("2: %x\n", (unsigned int)c);     // likewise
	printf("3: %x\n", (int)(char)c);        // likewise
	printf("4: %x\n", (int)(unsigned char)c);        // bitwise ;-)
	printf("5: %x\n", (int)((char)c & 0xff));        // strip sign-bit
	printf("6: %x\n", (int)(c & 0xff));              // likewise


	int x = 0xdeadbeef;
	unsigned int y = x;

#define bval(v, pos) (((unsigned char *)&v)[pos])

	printf("%.2x%.2x%.2x%.2x\n", bval(x, 0), bval(x, 1), bval(x, 2), bval(x, 3));
	printf("%.2x%.2x%.2x%.2x\n", bval(y, 0), bval(y, 1), bval(y, 2), bval(y, 3));

	/* read stdin
	 */
     
	for (;0;) {
		rsz = read(0, buf, sizeof(buf));
		if (rsz == -1) {
			printf("got -1, exiting, errno: %d\n", errno);
			return 0;
		}
		if (rsz == 0) {
			usleep(15000);  // 15ms sleep
		} else {
		  printf("read %d bytes\n", (int)rsz);
		}
	}

	return 0;
}
