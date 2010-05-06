#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"
#include "higherc/pbuf.h"

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define FATAL(...)       fatal(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define FATAL(fmt...)   fatal(__FILE__, __LINE__, fmt)
#endif

static void fatal(char *file, int line, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);

	fprintf(stderr, "fatal-error: %s: %i: ", file, (int)line);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);

	exit(-1);
}

struct mydata {
	int alpha;
	double bravo;
	char charlie[100];
};

int main(int argc, char **argv)
{
	int len = 10;
	int itemsiz = sizeof(struct mydata);
	struct hcns(pbuf) pbuf = HIGHERC_NULL_PBUF;

	if (!hcns(pbuf_alloc)(&pbuf, len, itemsiz)) {
		FATAL("failed to create a pbuf with %i items and %i bytes item size", len, itemsiz);
	}

	{
		int i, j;

		j = 2;
		while (j--) {
		    /* enqueue up to len
		     */

		    printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf.next, pbuf.enqueued, pbuf.list->length,
			   hcns(remaining)(&pbuf));

		    for (i=0; i<len; i++) {
			    assert(pbuf.next == i);
			    void *p = hcns(enqueue)(&pbuf);
			    printf("enqueued %p, remaining %i, next %i\n", p, hcns(remaining)(&pbuf), pbuf.next);
		    }

		    printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf.next, pbuf.enqueued, pbuf.list->length,
			   hcns(remaining)(&pbuf));

		    assert(hcns(remaining)(&pbuf) == 0);

		    /* dequeue up to len
		     */

		    for (i=0; i<len; i++) {
			    assert(hcns(remaining)(&pbuf) == i);
			    void *p = hcns(dequeue)(&pbuf);
			    printf("dequeued %p, remaining %i, next %i\n", p, hcns(remaining)(&pbuf), pbuf.next);
		    }

    		    assert(hcns(remaining)(&pbuf) == len);
		}

		/*
		 */

		assert(len >= 5);

		j = 5;
		while (j--) {
		    /* enqueue up to len
		     */

		    printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf.next, pbuf.enqueued, pbuf.list->length,
			   hcns(remaining)(&pbuf));

		    for (i=0; i<3; i++) {
			    void *p = hcns(enqueue)(&pbuf);
			    printf("enqueued %p, remaining %i, next %i\n", p, hcns(remaining)(&pbuf), pbuf.next);
		    }

		    printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf.next, pbuf.enqueued, pbuf.list->length,
			   hcns(remaining)(&pbuf));

		    /* dequeue up to len
		     */

		    for (i=0; i<3; i++) {
			    void *p = hcns(dequeue)(&pbuf);
			    printf("dequeued %p, remaining %i, next %i\n", p, hcns(remaining)(&pbuf), pbuf.next);
		    }

		    if (j == 0) {
			    printf("debug: next=%i enqueued=%i len=%i rem=%i\n", pbuf.next, pbuf.enqueued, pbuf.list->length, hcns(remaining)(&pbuf));
		    }
		}
	}

	assert(hcns(pbuf_free)(&pbuf) == 1);

	return 0;
}
