#ifndef tid0312e910d3c3t7zzl4zg0fut3i889en4d5dlshki6gd /* higherc-stralloc-h */
#define tid0312e910d3c3t7zzl4zg0fut3i889en4d5dlshki6gd /* higherc-stralloc-h */

struct stralloc {
	char *s;
	int len;
	int a;   /* allocated */
};

int hcns(stralloc_ready)(struct stralloc *, int);
int hcns(stralloc_readyplus)(struct stralloc *, int);
int hcns(stralloc_copy)(struct stralloc *, const struct stralloc *);
int hcns(stralloc_cat)(struct stralloc *, const struct stralloc *);
int hcns(stralloc_copys)(struct stralloc *, const char *);
int hcns(stralloc_cats)(struct stralloc *, const char *);
int hcns(stralloc_copyb)(struct stralloc *, const char *, int);
int hcns(stralloc_catb)(struct stralloc *, const char *, int);
int hcns(stralloc_append)(struct stralloc *, const char *);	/* beware: this takes a pointer to 1 char */
int hcns(stralloc_starts)(struct stralloc *, const char *);
int hcns(stralloc_catulong0)(struct stralloc *, long, int);
int hcns(stralloc_catlong0)(struct stralloc *, long, int);

/* #define stralloc_0(sa) stralloc_append(sa,"") */
/* #define stralloc_clear(sa) stralloc_copyb(sa, (char*)0, 0) */

/* #define stralloc_catulong(sa,l) (stralloc_catulong0((sa),(l),0)) */
/* #define stralloc_catlong(sa,l) (stralloc_catlong0((sa),(l),0)) */

/* #define stralloc_catuint0(sa,i,n) (stralloc_catulong0((sa),(i),(n))) */
/* #define stralloc_catint0(sa,i,n) (stralloc_catlong0((sa),(i),(n))) */
/* #define stralloc_catint(sa,i) (stralloc_catlong0((sa),(i),0)) */

#endif
