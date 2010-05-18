#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include "higherc/higherc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"
#include "higherc/rand.h"
#include "higherc/n.h"

/* a = b * c
 */

void hcns(d_mul2)(
	hcns(f) a[2],
	hcns(f) b,
	hcns(f) c)
{
	hcns(f) r, s, t, u, bh, bl, ch, cl;

	bh = HC_HIGH(b);
	bl = HC_LOW(b);
	ch = HC_HIGH(c);
	cl = HC_LOW(c);

	r = bl * cl;
	s = bh * ch;
	t = HC_HIGH(r) + bl * ch;
	u = t + (bh * cl);

	if (u < t) {
		s = s + HC_TO_HIGH(1);
	}

	a[0] = HC_LOW(r) + HC_TO_HIGH(u);
	a[1] = s + HC_HIGH(u);
}

int main(int argc, char **argv)
{

	assert(sizeof(hcns(u2)) == 2);
	assert(sizeof(hcns(u4)) == 4);
//	assert(sizeof(hcns(u8)) == 8);

	hcns(f) a[2];
	hcns(f) b = HC_F(0xdeadbeef);
	hcns(f) c = HC_F(0xdeadbeef);

	hcns(d_mul2)(a, b, c);

	printf(HC_FMT_H HC_FMT_H "\n", a[0], a[1]);

	return 0;
}
