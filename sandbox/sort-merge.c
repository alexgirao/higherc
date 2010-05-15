#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../higherc/higherc.h"
#include "../higherc/rand.h"

/* merge sort algorithm from:
 *  - http://en.wikipedia.org/wiki/Merge_sort
 */

/* algorithm: divide and conquer with balance line
 */

void merge_sort(int *a, int len)
{
	int mid = len / 2;

	

	printf("mid = %i\n", mid);

	printf("wip\n");
}

int main(int argc, char **argv)
{
	int a[30];
	int n, i;
	struct hcns(entropy) e;

	hcns(time_seed)(&e);

	n = sizeof(a) / sizeof(a[0]);
	
	for(i=0; i<n; i++) {
		//printf("%2i: %10d\n", i, a[i]);
		a[i] = hcns(rand4)(&e);
	}

	merge_sort(a, n);

	for(i=0; i<n; i++) {
		printf("%2i: %10d\n", i, a[i]);
	}

	return 0;
}
