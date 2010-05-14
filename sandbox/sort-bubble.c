#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* bubble sort from:
 *  - http://en.wikipedia.org/wiki/Bubble_sort
 */

/* algorithm: Bubble sort is a simple sorting algorithm. It works by
 *   repeatedly stepping through the list to be sorted, comparing each
 *   pair of adjacent items and swapping them if they are in the wrong
 *   order. The pass through the list is repeated until no swaps are
 *   needed, which indicates that the list is sorted.
 */

void bubble_sort(int *a, int len)
{
	int x, y, nswaps;
	do {
		nswaps = 0;

		x = 0;
		y = 1;

		printf("scanning\n");

		while (y < len) {
			if (a[x] > a[y]) {
				int t = a[x];
				a[x] = a[y];
				a[y] = t;
				printf("swap: %i <-> %i\n", x, y);
				nswaps++;
			}
			x++;
			y++;
		}

	} while (nswaps != 0);
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

	bubble_sort(a, n);

	for(i=0; i<n; i++) {
		printf("%2i: %10d\n", i, a[i]);
	}

	return 0;
}
