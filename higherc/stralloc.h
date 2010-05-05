#ifndef tid0312e910d3c3t7zzl4zg0fut3i889en4d5dlshki6gd /* higherc-stralloc-h */
#define tid0312e910d3c3t7zzl4zg0fut3i889en4d5dlshki6gd /* higherc-stralloc-h */

struct hcns(stralloc) {
	char *s;
	int len;
	int a;   /* allocated */
};

hcns(bool) hcns(stralloc_alloc)(struct hcns(stralloc) *x, int n);
hcns(bool) hcns(stralloc_free)(struct hcns(stralloc) *x);

hcns(bool) hcns(stralloc_copyn)(struct hcns(stralloc) *, const char *, int);
hcns(bool) hcns(stralloc_copy)(struct hcns(stralloc) *, const struct hcns(stralloc) *);
hcns(bool) hcns(stralloc_copyz)(struct hcns(stralloc) *, const char *);

hcns(bool) hcns(stralloc_catn)(struct hcns(stralloc) *, const char *, int);
hcns(bool) hcns(stralloc_cat)(struct hcns(stralloc) *, const struct hcns(stralloc) *);
hcns(bool) hcns(stralloc_catz)(struct hcns(stralloc) *, const char *);

#endif
