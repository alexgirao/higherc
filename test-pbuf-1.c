#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "higherc/higherc.h"
#include "higherc/list.h"
#include "higherc/pbuf.h"

struct mydata {
	int alpha;
	double bravo;
	char charlie[100];
};

int main(int argc, char **argv)
{
	int len = 10;
	int itemsiz = sizeof(struct mydata);
	int i;
	struct mydata *p;
	HC_DEF_PBUF(pbuf);

	hcns(pbuf_alloc)(pbuf, len, itemsiz);

	for (i=0; i<len; i++) {
		p = hcns(enqueue)(pbuf);
		p->alpha = i;
	}

	assert(p == hcns(shift)(pbuf));
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(shift)(pbuf))->alpha == pbuf->enqueued);
	assert(pbuf->enqueued == 0);
	assert(hcns(shift)(pbuf) == NULL);

	hcns(pbuf_free)(pbuf);

	return 0;
}
