#include "higherc/higherc.h"
#include "higherc/byte.h"

int hcns(byte_chrl)(const char *s, int n, int c)
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
			return t - s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - s;
		++t;

		if (t==u)
			break;
		if (*t == ch)
			return t - s;
		++t;
	}
	return n;
}

int hcns(byte_chrr)(const char *s, int n, int c)
{
	char ch;
	const char *t;

	ch = c;
	t = s + n;
	for (;;) {
		--t;
		if (t<s)
			break;
		if (*t == ch)
			return  t - s;

		--t;
		if (t<s)
			break;
		if (*t == ch)
			return  t - s;

		--t;
		if (t<s)
			break;
		if (*t == ch)
			return  t - s;

		--t;
		if (t<s)
			break;
		if (*t == ch)
			return  t - s;
	}
	return n;
}

void hcns(byte_copyl)(char *to, int n, const char *from)
{
	for (;;) {
		if (!n)
			return;
		*to++ = *from++;
		--n;

		if (!n)
			return;
		*to++ = *from++;
		--n;

		if (!n)
			return;
		*to++ = *from++;
		--n;

		if (!n)
			return;
		*to++ = *from++;
		--n;
	}
}

void hcns(byte_copyr)(char *to, int n, const char *from)
{
	to += n;
	from += n;
	for (;;) {
		if (!n)
			return;
		*--to = *--from;
		--n;

		if (!n)
			return;
		*--to = *--from;
		--n;

		if (!n)
			return;
		*--to = *--from;
		--n;

		if (!n)
			return;
		*--to = *--from;
		--n;
	}
}

int hcns(byte_diff)(const char *s, int n, const char *t)
{
	for (;;) {
		if (!n)
			return 0;
		if (*s != *t)
			break;
		++s;
		++t;
		--n;

		if (!n)
			return 0;
		if (*s != *t)
			break;
		++s;
		++t;
		--n;

		if (!n)
			return 0;
		if (*s != *t)
			break;
		++s;
		++t;
		--n;

		if (!n)
			return 0;
		if (*s != *t)
			break;
		++s;
		++t;
		--n;
	}
	return ((int)(unsigned char)*s)
	    - ((int)(unsigned char)*t);
}

void hcns(byte_zero)(char *s, int n)
{
	for (;;) {
		if (!n)
			break;
		*s++ = 0;
		--n;

		if (!n)
			break;
		*s++ = 0;
		--n;

		if (!n)
			break;
		*s++ = 0;
		--n;

		if (!n)
			break;
		*s++ = 0;
		--n;
	}
}
