#include <time.h>

#include "higherc/higherc.h"
#include "higherc/rand.h"

void hcns(seed)(struct hcns(entropy) *e, int seed)
{
	e->entropy = seed;
}

void hcns(time_seed)(struct hcns(entropy) *e)
{
	e->entropy = time(NULL);
}

int hcns(rand4)(struct hcns(entropy) *e)
{
	int tmp = e->entropy * HC_RAND4_MAGIC + HC_RAND4_MAGIC;
	e->entropy = tmp + (tmp >> (5 + 16)) + (tmp >> (8 + 16)) + (tmp >> (13 + 16));
	return e->entropy & HC_RAND4_MAX;
}
