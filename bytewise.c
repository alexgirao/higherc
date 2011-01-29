/*
 * Copyright (C) 2010 Alexandre Girao <alexgirao@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "higherc/higherc.h"
#include "higherc/byte.h"
#include "higherc/bytewise.h"

const signed char hcns(hexval_table)[256] = {
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 00-07 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 08-0f */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 10-17 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 18-1f */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 20-27 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 28-2f */
	  0,  1,  2,  3,  4,  5,  6,  7,		/* 30-37 */
	  8,  9, -1, -1, -1, -1, -1, -1,		/* 38-3f */
	 -1, 10, 11, 12, 13, 14, 15, -1,		/* 40-47 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 48-4f */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 50-57 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 58-5f */
	 -1, 10, 11, 12, 13, 14, 15, -1,		/* 60-67 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 68-67 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 70-77 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 78-7f */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 80-87 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 88-8f */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 90-97 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* 98-9f */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* a0-a7 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* a8-af */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* b0-b7 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* b8-bf */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* c0-c7 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* c8-cf */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* d0-d7 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* d8-df */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* e0-e7 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* e8-ef */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* f0-f7 */
	 -1, -1, -1, -1, -1, -1, -1, -1,		/* f8-ff */
};

/*
 */

enum {
	S = HC_SPACE,
	P = HC_PUNCT,
	/* composite type
	 */
	A = HC_ALPHA | HC_ALNUM,
	D = HC_DIGIT | HC_ALNUM,
	C = HC_SPACE | HC_BLANK,
	B = HC_GLOB | HC_REGEX_META | HC_PUNCT,
	R = HC_REGEX_META | HC_PUNCT,
	H = HC_REGEX_CHAR | HC_PUNCT,
	I = HC_GLOB | HC_REGEX_CHAR | HC_PUNCT,
	J = HC_GLOB | HC_REGEX_META | HC_REGEX_CHAR | HC_PUNCT,
	E = HC_REGEX_META | HC_REGEX_CHAR | HC_PUNCT
};

hcns(u2) hcns(ctypetbl)[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, C, S, S, S, S, 0, 0,		/*   0.. 15 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,		/*  16.. 31 */
	C, P, P, P, R, P, P, P, R, R, B, R, P, H, R, P,		/*  32.. 47 */
	D, D, D, D, D, D, D, D, D, D, P, P, P, P, P, B,		/*  48.. 63 */
	P, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,		/*  64.. 79 */
	A, A, A, A, A, A, A, A, A, A, A, J, J, H, E, P,		/*  80.. 95 */
	P, A, A, A, A, A, A, A, A, A, A, A, A, A, A, A,		/*  96..111 */
	A, A, A, A, A, A, A, A, A, A, A, R, R, P, P, 0,		/* 112..127 */
	/* Nothing in the 128.. range */
};

/*
 * big-endian, 7-bit encoding, msb is set at last digit
 *
 * len0 needs at least 5 digits
 *
 */
int hcns(enc_u4_be_7x8)(void *len0, hcns(u4) v)
{
	hcns(u1) *len=len0;
	hcns(u4) q=v, r;

	if (q) {
		if (len0) {
			while (q) {
				r = q & 0x7f;
				q = q >> 7;
				*len++ = r;
			}
			((hcns(u1)*)len0)[0] |= 0x80; /* this will be the last digit */
			hcns(brev)(len0, len - (hcns(u1)*)len0); /* make big-endian (simpler to decode) */
		} else {
			if ((q & ~HC_BITMASK(7)) == 0) {
				return 1;
			} else if ((q & ~HC_BITMASK(14)) == 0) {
				return 2;
			} else if ((q & ~HC_BITMASK(21)) == 0) {
				return 3;
			} else if ((q & ~HC_BITMASK(28)) == 0) {
				return 4;
			}
			return 5;
		}
	} else if (len0) {
		*len++ = 0x80;
	} else {
		return 1;
	}

	return len - (hcns(u1)*)len0;
}

hcns(u4) hcns(dec_u4_be_7x8)(void *len0, int *lenp)
{
	hcns(u1) *len=len0;
	hcns(u4) q=0, r;

#define ITER 						\
		r = *len++;				\
		if (r & 0x80) {				\
			q = (q << 7) | (r & 0x7f);	\
			break;				\
		} else {				\
			q = (q << 7) | r;		\
		}

	do {
		ITER;
		ITER;
		ITER;
		ITER;
		ITER;
	} while (0);

#undef ITER

	if (lenp) {
		*lenp = len - (hcns(u1)*)len0;
	}

	return q;
}

/*
 */
