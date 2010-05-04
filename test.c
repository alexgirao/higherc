#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "buffer.h"
#include "list.h"

int main(int argc, char **argv)
{
	char buf[1024];
	ssize_t rsz;

	/* read stdin
	 */
     
	for (;;) {
		rsz = read(0, buf, sizeof(buf));
		if (rsz == -1) {
			printf("got -1, exiting, errno: %d\n", errno);
			return 0;
		}
		if (rsz == 0) {
			usleep(15000);  // 15ms sleep
		} else {
			printf("read %d bytes\n", rsz);
		}
	}

	return 0;
}
