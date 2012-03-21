#ifndef tid030fbf737660l0zwyeopvkr8gjzzzn66o6r52281hsa /* higherc-tagid-h */
#define tid030fbf737660l0zwyeopvkr8gjzzzn66o6r52281hsa /* higherc-tagid-h */

#ifndef tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */
#error higherc-s-h not included
#endif

/* tag
 */

HC_DECL_PUBLIC_I_HEADERS(tag,
			 HC_ST_S value[1];
);

HC_DECL_PUBLIC_I_USORT_HEADER(tag);

#define HC_ST_TAG struct tag

struct tag *tag_new(struct tag *x, HC_ST_S *s);
struct tag *tag_newz(struct tag *x, char *z);

void tag_free(struct tag *x);

bool tag_is_valid(struct s *s);

bool tag_set(struct tag *x, HC_ST_S *s);
bool tag_setz(struct tag *x, char *z);

/* tagid
 */

struct tagid {
	int B;
	int C;
	unsigned int D;
	unsigned char E[20];  /* sha1 bytes */
};

#define HC_NULL_TAGID {0, 0, 0, {			\
			0, 0, 0, 0, 0,			\
				0, 0, 0, 0, 0,		\
				0, 0, 0, 0, 0,		\
				0, 0, 0, 0, 0		\
				}}

#define HC_ST_TAGID struct tagid
#define HC_DEF_TAGID(sym) HC_ST_TAGID sym[1] = {HC_NULL_TAGID}

void tagid_set_tags0(HC_ST_TAGID *tagid, HC_ST_S *A, HC_ST_TAG *tag);
void tagid_set_tags(HC_ST_TAGID *tagid, HC_ST_TAG *tag);
void tagid_free(HC_ST_TAGID *tagid);
void tagid_cat_id(HC_ST_TAGID *tagid, HC_ST_S *tid);

#define HC_TAGID_RAW_LEN 26

/* serialize tagid, write 26 bytes
 */
void tagid_put(HC_ST_TAGID *tagid, void *out);

/* deserialize tagid, read 26 bytes
 */
void tagid_get(HC_ST_TAGID *tagid, void *in);

#endif
