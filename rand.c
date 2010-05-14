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
	e->entropy = tmp + (tmp >> 8) + (tmp >> 13) + (tmp >> 21);
	return e->entropy & HC_RAND4_MAX;
}
