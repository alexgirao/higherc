#ifndef tid030fbf737660l0zwyeopvkr8gjzzzn66o6r52281hsa /* higherc-tagid-h */
#define tid030fbf737660l0zwyeopvkr8gjzzzn66o6r52281hsa /* higherc-tagid-h */

HC_DECL_PUBLIC_I_HEADERS(hcns(tag),
			 HC_ST_S value[1];
);

HC_DECL_PUBLIC_I_USORT_HEADER(hcns(tag));

#define HC_ST_TAG struct hcns(tag)

struct hcns(tag) *hcns(tag_new)(struct hcns(tag) *x, char *z);
void hcns(tag_free)(struct hcns(tag) *x);

hcns(bool) hcns(tag_is_valid)(struct hcns(s) *s);
hcns(bool) hcns(tag_setz)(struct hcns(tag) *x, char *z);

#endif
