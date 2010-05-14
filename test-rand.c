#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
#include "higherc/rand.h"

int main()
{
	struct hcns(entropy) e;
	struct hcns(entropy) e2;
	int i;

	hcns(time_seed)(&e);

	e2.entropy = e.entropy + 1;

	for (i=0; i<30; i++) {
		printf("%10i %10i\n", hcns(rand4)(&e), hcns(rand4)(&e2));
	}

	return 0;
}
