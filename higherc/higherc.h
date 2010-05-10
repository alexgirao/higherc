#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#define tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */

#ifndef hcns /* higherc namespace */
#define hcns(v) higherc_##v
#endif

typedef int hcns(bool);   /* 0 = false, 1 = true */

#define HC_OFFSET(p, c)                ((char*)(p)+(int)(c))

#define HC_ALIGN_BY(bytes,alignment)       ((bytes + alignment) & ~(alignment-1))
#define HC_ALIGN4(bytes)                   HC_ALIGN_BY(bytes,4)
#define HC_ALIGN8(bytes)                   HC_ALIGN_BY(bytes,8)
#define HC_ALIGN16(bytes)                  HC_ALIGN_BY(bytes,16)

#endif
