#ifndef tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */
#define tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */

struct hcns(s) {
	char *s;
	int len; /* can be changed between 0 and a-1 (inclusive) to truncate string */
	int a; /* allocated */
};

#define HC_NULL_S {NULL, 0, 0}

hcns(bool) hcns(s_alloc)(struct hcns(s) *x, int n);
hcns(bool) hcns(s_free)(struct hcns(s) *x);

hcns(bool) hcns(s_copyn)(struct hcns(s) *, const char *, int);
hcns(bool) hcns(s_copy)(struct hcns(s) *, const struct hcns(s) *);
hcns(bool) hcns(s_copyz)(struct hcns(s) *, const char *);

hcns(bool) hcns(s_catn)(struct hcns(s) *, const char *, int);
hcns(bool) hcns(s_cat)(struct hcns(s) *, const struct hcns(s) *);
hcns(bool) hcns(s_catz)(struct hcns(s) *, const char *);

int hcns(s_bdiff)(struct hcns(s) *a, char *b, int bl);
int hcns(s_sdiff)(struct hcns(s) *a, char *b);

void hcns(s_upper)(struct hcns(s) *s);
void hcns(s_lower)(struct hcns(s) *s);

#define HC_SAFE_CSTR(s) do { hcns(s_catn)(s, "\0", 1); --(s)->len; } while (0)

#endif
