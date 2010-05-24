#ifndef tid030fbf737660l0zwyeopvkr8gjzzzn66o6r52281hsa /* higherc-tagid-h */
#define tid030fbf737660l0zwyeopvkr8gjzzzn66o6r52281hsa /* higherc-tagid-h */

#ifndef tid030b8ac9b6bbmv6xgcvtam0vl49kzcezeu1cy2wuzd5 /* higherc-s-h */
#error higherc-s-h not included
#endif

/* tag
 */

HC_DECL_PUBLIC_I_HEADERS(hcns(tag),
			 HC_ST_S value[1];
);

HC_DECL_PUBLIC_I_USORT_HEADER(hcns(tag));

#define HC_ST_TAG struct hcns(tag)

struct hcns(tag) *hcns(tag_new)(struct hcns(tag) *x, HC_ST_S *s);
struct hcns(tag) *hcns(tag_newz)(struct hcns(tag) *x, char *z);

void hcns(tag_free)(struct hcns(tag) *x);

hcns(bool) hcns(tag_is_valid)(struct hcns(s) *s);

hcns(bool) hcns(tag_set)(struct hcns(tag) *x, HC_ST_S *s);
hcns(bool) hcns(tag_setz)(struct hcns(tag) *x, char *z);

/* tagid
 */

struct hcns(tagid) {
	HC_ST_S A[1];
	int B;
	int C;
	unsigned int D;
	unsigned char E[20];  /* sha1 bytes */
};

#define HC_NULL_TAGID {{HC_NULL_S}, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define HC_ST_TAGID struct hcns(tagid)
#define HC_DEF_TAGID(sym) HC_ST_TAGID sym[1] = {HC_NULL_TAGID}

void hcns(tagid_set_tags)(HC_ST_TAGID *tagid, HC_ST_TAG *tag);
void hcns(tagid_free)(HC_ST_TAGID *tagid);
void hcns(tagid_cat_id)(HC_ST_TAGID *tagid, HC_ST_S *tid);

#endif
