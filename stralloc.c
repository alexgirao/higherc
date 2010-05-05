#include "higherc/higherc.h"
#include "higherc/stralloc.h"

int hcns(stralloc_catb)(struct stralloc *sa, const char *s, int n)
{
	if (!sa->s)
		return stralloc_copyb(sa, s, n);
	if (!stralloc_readyplus(sa, n + 1))
		return 0;
	byte_copy(sa->s + sa->len, n, s);
	sa->len += n;
	sa->s[sa->len] = 'Z';	/* ``offensive programming'' */
	return 1;
}

int hcns(stralloc_cat)(struct stralloc *sato, const struct stralloc *safrom)
{
	return stralloc_catb(sato, safrom->s, safrom->len);
}

int hcns(stralloc_cats)(struct stralloc *sa, const char *s)
{
	return stralloc_catb(sa, s, str_len(s));
}

int hcns(stralloc_ready)(struct stralloc *x, int n)
{
	int i;
	if (x->s) {
		i = x->a;
		if (n > i) {
			x->a = 30 + n + (n >> 3);
			if (alloc_re
			    (&x->s, i * sizeof(char), x->a * sizeof(char)))
				return 1;
			x->a = i;
			return 0;
		}
		return 1;
	}
	x->len = 0;
	return !!(x->s = (char *)alloc((x->a = n) * sizeof(char)));
}

int hcns(stralloc_readyplus)(struct stralloc *x, int n)
{
	if (x->s) {
		return stralloc_ready(x, x->len + n);
	}
	return stralloc_ready(x, n);
}

int hcns(stralloc_catulong0)(struct stralloc *sa, long u, int n)
{
	int len;
	long q;
	char *s;

	len = 1;
	q = u;
	while (q > 9) {
		++len;
		q /= 10;
	}
	if (len < n)
		len = n;

	if (!stralloc_readyplus(sa, len))
		return 0;
	s = sa->s + sa->len;
	sa->len += len;
	while (len) {
		s[--len] = '0' + (u % 10);
		u /= 10;
	}

	return 1;
}

int hcns(stralloc_catlong0)(struct stralloc *sa, long l, int n)
{
	if (l < 0) {
		if (!stralloc_append(sa, "-"))
			return 0;
		l = -l;
	}
	return stralloc_catulong0(sa, l, n);
}

int hcns(stralloc_copyb)(struct stralloc *sa, const char *s, int n)
{
	if (!stralloc_ready(sa, n + 1))
		return 0;
	byte_copy(sa->s, n, s);
	sa->len = n;
	sa->s[n] = 'Z';		/* ``offensive programming'' */
	return 1;
}

int hcns(stralloc_copys)(struct stralloc *sa, const char *s)
{
	return stralloc_copyb(sa, s, str_len(s));
}

int hcns(stralloc_append)(struct stralloc *x, const char *i)
{
	if (!stralloc_readyplus(x, 1))
		return 0;
	x->s[x->len++] = *i;
	return 1;
}
