#include "higherc/higherc.h"
#include "higherc/str.h"

int hcns(str_chr)(const char *s, int c)
{
	char ch;
	const char *t;

	ch = c;
	t = s;
	for (;;) {
		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			break;
		++t;

	}
	return t - s;
}

int hcns(str_copy)(char *s, const char *t)
{
	char *tmp = s;
	for (;;) {
		if (!(*s = *t))
			break;
		++s;
		++t;

		if (!(*s = *t))
			break;
		++s;
		++t;

		if (!(*s = *t))
			break;
		++s;
		++t;
		
		if (!(*s = *t))
			break;
		++s;
		++t;
	}
	return s - tmp;
}

int hcns(str_diff)(const char *s, const char *t)
{
	char x;

	for (;;) {
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;
	}

	return ((int)(unsigned char)x)
	    - ((int)(unsigned char)*t);
}

int hcns(str_diffn)(const char *s, const char *t, int len)
{
	char x;

	for (;;) {
		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;

		if (!len--)
			return 0;
		x = *s;
		if (x != *t)
			break;
		if (!x)
			break;
		++s;
		++t;
	}

	return ((int)(unsigned char)x)
	    - ((int)(unsigned char)*t);
}

int hcns(str_len)(const char *s)
{
	const char *t;

	t = s;
	for (;;) {
		if (!*t)
			return t - s;
		++t;

		if (!*t)
			return t - s;
		++t;

		if (!*t)
			return t - s;
		++t;

		if (!*t)
			return t - s;
		++t;
	}
}

int hcns(str_rchr)(const char *s, int c)
{
	char ch;
	const char *t;
	const char *u;

	ch = c;
	t = s;
	u = 0;
	for (;;) {
		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;

		if (!*t)
			break;
		if (*t == ch)
			u = t;
		++t;
	}
	if (u)
		return u - s;
	return t - s;
}

int hcns(str_start)(const char *s, const char *t)
{
	char x;

	for (;;) {
		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;

		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;

		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;

		x = *t++;
		if (!x)
			return 1;
		if (x != *s++)
			return 0;
	}
}
