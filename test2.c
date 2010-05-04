#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc.h"
#include "buffer.h"
#include "list.h"

struct mydata {
	int alpha;
	double bravo;
	char charlie[100];
};

int main(int argc, char **argv)
{
	struct hcns(list) *list = hcns(list_alloc)(10, 10 * sizeof(struct mydata), NULL);

	

	hcns(list_free)(list);

	return 0;
}
