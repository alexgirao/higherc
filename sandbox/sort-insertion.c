#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* insertion sort algorithm from:
 *  - http://electrofriends.com/source-codes/software-programs/c/sorting-programs/program-to-sort-the-numbers-using-insertion-sort/
 *  - http://en.wikipedia.org/wiki/Insertion_sort
 */

/* algorithm: Every iteration of insertion sort removes an element
 *   from the input data, inserting it into the correct position in
 *   the already-sorted list, until no input elements remain. The
 *   choice of which element to remove from the input is arbitrary,
 *   and can be made using almost any choice algorithm.
 */

void insertion_sort(int *a, int len)
{
	int i, input;
	for(i=1; i<len; i++) {
		int j = i - 1;

		printf("input %i\n", i);

		input = a[i];

		while(j >= 0 && a[j] > input) {
			/* shift down elements greater than input
			 */
			printf("shift: %i -> %i\n", j, j+1);
			a[j+1] = a[j];
			j--;
		}

		if (j+1 != i) {
			/* don't swap if input is already in order
			 */
			printf("move input to %i\n", j+1);
			a[j+1] = input;
		}
	}
	printf("done\n");
}

int main(int argc, char **argv)
{
	int a[] = {5, 7, 0, 3, 4, 2, 6, 1};
	int n, i;

	n = sizeof(a) / sizeof(a[0]);
	
	for(i=0; i<n; i++) {
		printf("%2i: %10d\n", i, a[i]);
	}

	insertion_sort(a, n);

	for(i=0; i<n; i++) {
		printf("%2i: %10d\n", i, a[i]);
	}

	return 0;
}
