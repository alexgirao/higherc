#include "higherc/higherc.h"
#include "higherc/str.h"

int hcns(schrl)(const char *s, int c)
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

int hcns(schrr)(const char *s, int c)
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

int hcns(scopy)(char *s, const char *t)
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

int hcns(sdiff)(const char *s, const char *t)
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

int hcns(sdiffn)(const char *s, const char *t, int len)
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

int hcns(slen)(const char *s)
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

hcns(bool) hcns(sstart)(const char *s, const char *t)
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
