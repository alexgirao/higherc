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
	int i, j, pos;
	struct mydata *p;
	HC_DEF_PBUF(pbuf);

	hcns(pbuf_alloc)(pbuf, len, itemsiz);

	for (i=0; i<len; i++) {
		p = hcns(pbuf_enqueue)(pbuf);
		p->alpha = i;
	}

	assert(len == 10);

	assert(p == hcns(pbuf_shift)(pbuf));
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == pbuf->enqueued);

	assert(pbuf->enqueued == 0);
	assert(hcns(pbuf_shift)(pbuf) == NULL);

	/* fillup again, up to 7
	 */

	for (i=0; i<7; i++) {
		p = hcns(pbuf_enqueue)(pbuf);
		p->alpha = i * 100;
	}

	assert(((struct mydata *)hcns(pbuf_dequeue)(pbuf))->alpha == 0);
	assert(((struct mydata *)hcns(pbuf_dequeue)(pbuf))->alpha == 100);
	assert(((struct mydata *)hcns(pbuf_shift)(pbuf))->alpha == 600);

	/* traverse queued slots
	 */

	i = 0;
	j = hcns(pbuf_enqueued_len)(pbuf);
	pos = hcns(pbuf_enqueued_first)(pbuf);
	for (; i<j; i++) {
		p = hcns(pbuf_item)(pbuf, pos);
		pos = hcns(pbuf_next)(pbuf, pos);
		printf("enqueued[%i] = %i\n", pos, p->alpha);
	}

	/* traverse remaining items
	 */

	i = 0;
	j = hcns(pbuf_remaining_len)(pbuf);
	pos = hcns(pbuf_remaining_first)(pbuf);
	for (; i<j; i++) {
		p = hcns(pbuf_item)(pbuf, pos);
		pos = hcns(pbuf_next)(pbuf, pos);
		printf("remaining[%i] = %i\n", pos, p->alpha);
	}

	/* traverse queued slots (backward)
	 */

	i = 0;
	j = hcns(pbuf_enqueued_len)(pbuf);
	pos = hcns(pbuf_enqueued_last)(pbuf);
	for (; i<j; i++) {
		p = hcns(pbuf_item)(pbuf, pos);
		pos = hcns(pbuf_prior)(pbuf, pos);
		printf("enqueued[%i] = %i\n", pos, p->alpha);
	}

	/* traverse remaining items (backward)
	 */

	i = 0;
	j = hcns(pbuf_remaining_len)(pbuf);
	pos = hcns(pbuf_remaining_last)(pbuf);
	for (; i<j; i++) {
		p = hcns(pbuf_item)(pbuf, pos);
		pos = hcns(pbuf_prior)(pbuf, pos);
		printf("remaining[%i] = %i\n", pos, p->alpha);
	}

	hcns(pbuf_free)(pbuf);

	return 0;
}
