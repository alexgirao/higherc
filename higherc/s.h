#ifndef tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */
#define tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

struct s {
	char *s;
	int len; /* can be changed between 0 and a-1 (inclusive) to truncate string */
	int a; /* allocated */
};

#define HC_NULL_S {NULL, 0, 0}
#define HC_ST_S struct s
#define HC_DEF_S(sym) HC_ST_S sym[1] = {HC_NULL_S}

void s_alloc(HC_ST_S *x, int n);
void s_free(HC_ST_S *x);

void s_copyn(HC_ST_S *, const char *, int);
void s_copy(HC_ST_S *, const HC_ST_S *);
void s_copyz(HC_ST_S *, const char *);
void s_copyc(HC_ST_S *sa, int c);

void s_catn(HC_ST_S *, const char *, int);
void s_cat(HC_ST_S *, const HC_ST_S *);
void s_catz(HC_ST_S *, const char *);
void s_catc(HC_ST_S *sa, int c);

void s_vformat(HC_ST_S *sa, bool cat, const char *fmt, va_list va);
void s_copyf(HC_ST_S *sa, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
void s_catf(HC_ST_S *sa, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));

/* s_cat_* return amount of written digits and sign '-'
 */
int s_cat_i4_hex(HC_ST_S *, int32_t);
int s_cat_u4_hex(HC_ST_S *, uint32_t);
int s_cat_i4_dec(HC_ST_S *, int32_t);
int s_cat_u4_dec(HC_ST_S *, uint32_t);
int s_cat_i4_base36(HC_ST_S *, int32_t);
int s_cat_u4_base36(HC_ST_S *, uint32_t);

int s_cat_sha1hex(HC_ST_S *x, void *s, int n);

int s_diffn(HC_ST_S *a, char *b, int bl);
int s_diff(HC_ST_S *a, HC_ST_S *b);
int s_diffz(HC_ST_S *a, char *b);

void s_upper(HC_ST_S *s);
void s_lower(HC_ST_S *s);

int s_lchr(HC_ST_S *s, int chr);
int s_rchr(HC_ST_S *s, int chr);

/*
 * s_shiftr:
 *   start .. end range: 0 .. ? (exclusive, -1 allowed for both (means: len - abs(end)))
 * s_shiftl:
 *   start .. end range: 0 .. len (exclusive, -1 allowed for both (means: len - abs(end)))
 * s_shiftr2:
 *   likewise, end is always len + n (always expand)
 * s_shiftl2:
 *   likewise, end is always len (shift from end of string)
 */

void s_shiftr(HC_ST_S *s, int start, int end, int n, int pad);
void s_shiftl(HC_ST_S *s, int start, int end, int n, int pad);
void s_shiftr2(HC_ST_S *s, int start, int n, int pad);
void s_shiftl2(HC_ST_S *s, int start, int n, int pad);

#define HC_SAFE_CSTR(s) do {s_catc(s, '\0'); --(s)->len;} while (0)

void s_reprn(HC_ST_S *s_repr, void *s, int n);
void s_repr(HC_ST_S *s_repr, HC_ST_S *s);
void s_reprz(HC_ST_S *s_repr, char *s);

/* serialization
 */

int s_putlen(HC_ST_S *x);

/* return bytes written
 */
int s_put(HC_ST_S *x, void *out);

/* return bytes read
 */
int s_get(HC_ST_S *x, void *in);

#endif
