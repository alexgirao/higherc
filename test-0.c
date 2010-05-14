#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"
#include "higherc/rand.h"

int ddddddddddddd_main(int argc, char **argv)
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

//#include <>

int main()
{
	int A[20], N, Temp, i, j;
	struct hcns(entropy) e;

	hcns(time_seed)(&e);

	N = sizeof(A) / sizeof(A[0]);
	for(i=0; i<N; i++) {
		A[i] = hcns(rand4)(&e);
	}
	
	for(i=1; i<N; i++)
	{
		Temp = A[i];
		j = i-1;
		fflush(stdout);
		while(Temp < A[j] && j >= 0) {
			A[j+1] = A[j];
			j = j-1;
		}
		A[j+1] = Temp;
	}

	for(i=0; i<N; i++) {
		printf("%10d\n", A[i]);
	}

	return 0;
}
