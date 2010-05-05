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

	printf("1: %x\n", (int)c);              // calculate two's complement
	printf("2: %x\n", (unsigned int)c);     // calculate two's complement (even if unsigned)
	printf("3: %x\n", (int)(char)c);        // calculate two's complement (even if unsigned)
	printf("4: %x\n", (int)(unsigned char)c);        // ok (most right form?)
	printf("5: %x\n", (int)((char)c & 0xff));        // ok
	printf("6: %x\n", (int)(c & 0xff));              // ok

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
