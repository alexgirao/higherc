#ifndef tid0312e910d3c3t7zzl4zg0fut3i889en4d5dlshki6gd /* higherc-stralloc-h */
#define tid0312e910d3c3t7zzl4zg0fut3i889en4d5dlshki6gd /* higherc-stralloc-h */

struct hcns(s) {
	char *s;
	int len;
	int a;   /* allocated */
};

hcns(bool) hcns(s_alloc)(struct hcns(s) *x, int n);
hcns(bool) hcns(s_free)(struct hcns(s) *x);

hcns(bool) hcns(s_copyn)(struct hcns(s) *, const char *, int);
hcns(bool) hcns(s_copy)(struct hcns(s) *, const struct hcns(s) *);
hcns(bool) hcns(s_copyz)(struct hcns(s) *, const char *);

hcns(bool) hcns(s_catn)(struct hcns(s) *, const char *, int);
hcns(bool) hcns(s_cat)(struct hcns(s) *, const struct hcns(s) *);
hcns(bool) hcns(s_catz)(struct hcns(s) *, const char *);

#endif
