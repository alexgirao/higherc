#ifndef tid030e0d917b07m4dxiqcl0ri46t6yjalx6vdbq75qv5v /* higherc-sha1-h */
#define tid030e0d917b07m4dxiqcl0ri46t6yjalx6vdbq75qv5v /* higherc-sha1-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

/*
 * SHA1 routine optimized to do word accesses rather than byte accesses,
 * and to avoid unnecessary copies into the context array.
 *
 * This was initially based on the Mozilla SHA1 implementation, although
 * none of the original Mozilla code remains.
 */

struct hcns(sha1) {
	unsigned long long size;
	unsigned int H[5];
	unsigned int W[16];
};

#define HC_NULL_SHA1 {0LL, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
#define HC_ST_SHA1 struct hcns(sha1)
#define HC_DEF_SHA1(sym) HC_ST_SHA1 sym[1] = {HC_NULL_SHA1}

void hcns(sha1_init)(struct hcns(sha1) *ctx);
void hcns(sha1_update)(struct hcns(sha1) *ctx, const void *dataIn, unsigned long len);
void hcns(sha1_final)(struct hcns(sha1) *ctx, unsigned char hashout[20]);

#endif
