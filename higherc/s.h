#ifndef tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */
#define tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */

struct hcns(s) {
	char *s;
	int len; /* can be changed between 0 and a-1 (inclusive) to truncate string */
	int a; /* allocated */
};

#define HC_NULL_S {NULL, 0, 0}
#define HC_ST_S struct hcns(s)
#define HC_DEF_S(sym) HC_ST_S sym[1] = {HC_NULL_S}

void hcns(s_alloc)(HC_ST_S *x, int n);
void hcns(s_free)(HC_ST_S *x);

void hcns(s_copyn)(HC_ST_S *, const char *, int);
void hcns(s_copy)(HC_ST_S *, const HC_ST_S *);
void hcns(s_copyz)(HC_ST_S *, const char *);

void hcns(s_catn)(HC_ST_S *, const char *, int);
void hcns(s_cat)(HC_ST_S *, const HC_ST_S *);
void hcns(s_catz)(HC_ST_S *, const char *);

/* s_cat_* return amount of written digits and sign '-'
 */
int hcns(s_cat_i4_hex)(HC_ST_S *, hcns(i4));
int hcns(s_cat_u4_hex)(HC_ST_S *, hcns(u4));
int hcns(s_cat_i4_dec)(HC_ST_S *, hcns(i4));
int hcns(s_cat_u4_dec)(HC_ST_S *, hcns(u4));
int hcns(s_cat_i4_base36)(HC_ST_S *, hcns(i4));
int hcns(s_cat_u4_base36)(HC_ST_S *, hcns(u4));

int hcns(s_diffn)(HC_ST_S *a, char *b, int bl);
int hcns(s_diff)(HC_ST_S *a, HC_ST_S *b);
int hcns(s_diffz)(HC_ST_S *a, char *b);

void hcns(s_upper)(HC_ST_S *s);
void hcns(s_lower)(HC_ST_S *s);

#define HC_SAFE_CSTR(s) do {hcns(s_catn)(s, "\0", 1); --(s)->len;} while (0)

#endif
