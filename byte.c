#include "higherc/higherc.h"
#include "higherc/byte.h"

int hcns(bchrl)(const void *s, int n, int c)
{
	char ch;
	const char *t;
	const char *u;

	ch = c;
	t = s;
	u = s + n;

	for (;;) {
		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - (char*)s;
		++t;
	}
	return n;
}

int hcns(bchrr)(const void *s, int n, int c)
{
	char ch;
	const char *t;

	ch = c;
	t = s + n;
	for (;;) {
		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;

		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;

		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;

		--t;
		if (t<(char*)s)
			break;
		if (*t == ch)
			return  t - (char*)s;
	}
	return n;
}

void hcns(bcopyl)(void *to, int n, const void *from)
{
	char *t=to;
	const char *f=from;
	for (;;) {
		if (!n)
			return;
		*t++ = *f++;
		--n;

		if (!n)
			return;
		*t++ = *f++;
		--n;

		if (!n)
			return;
		*t++ = *f++;
		--n;

		if (!n)
			return;
		*t++ = *f++;
		--n;
	}
}

void hcns(bcopyr)(void *to, int n, const void *from)
{
	char *t=to;
	const char *f=from;
	to += n;
	from += n;
	for (;;) {
		if (!n)
			return;
		*--t = *--f;
		--n;

		if (!n)
			return;
		*--t = *--f;
		--n;

		if (!n)
			return;
		*--t = *--f;
		--n;

		if (!n)
			return;
		*--t = *--f;
		--n;
	}
}

int hcns(bdiff)(const void *s, int n, const void *t)
{
	const char *x=s;
	const char *y=t;
	for (;;) {
		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;

		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;

		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;

		if (!n)
			return 0;
		if (*x != *y)
			break;
		++x;
		++y;
		--n;
	}
	return ((int)(unsigned char)*x)
	    - ((int)(unsigned char)*y);
}

void hcns(bzero)(void *s, int n)
{
	char *x=s;
	for (;;) {
		if (!n)
			break;
		*x++ = 0;
		--n;

		if (!n)
			break;
		*x++ = 0;
		--n;

		if (!n)
			break;
		*x++ = 0;
		--n;

		if (!n)
			break;
		*x++ = 0;
		--n;
	}
}
