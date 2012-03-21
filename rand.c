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

#include <time.h>

#include "higherc/higherc.h"
#include "higherc/rand.h"

void seed(struct entropy *e, int seed)
{
	e->entropy = seed;
}

void time_seed(struct entropy *e)
{
	e->entropy = time(NULL);
}

int rand4(struct entropy *e)
{
	int tmp = e->entropy * HC_RAND4_MAGIC + HC_RAND4_MAGIC;
	e->entropy = tmp + (tmp >> (5 + 16)) + (tmp >> (8 + 16)) + (tmp >> (13 + 16));
	return e->entropy & HC_RAND4_MAX;
}
