#ifndef tid030e26659e595qsr1p0geeepwn1o4b6lfeein7dbro5 /* higherc-rand-h */
#define tid030e26659e595qsr1p0geeepwn1o4b6lfeein7dbro5 /* higherc-rand-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

#define HC_RAND4_MAX 0x7fffffff
#define HC_RAND4_MAGIC 1618034

struct hcns(entropy) {
	int entropy;
};

void hcns(seed)(struct hcns(entropy) *e, int seed);
void hcns(time_seed)(struct hcns(entropy) *e);
int hcns(rand4)(struct hcns(entropy) *e);

#endif
