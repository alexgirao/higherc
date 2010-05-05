#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#define tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */

#ifndef hcns /* higherc namespace */
#define hcns(v) higherc_##v
#endif

typedef int hcns(bool);   /* 0 = false, 1 = true */

#define higherc_offset(p, c)                ((char*)(p)+(int)(c))

#define higherc_align_by(bytes,alignment)       ((bytes + alignment) & ~(alignment-1))
#define higherc_align4(bytes)                   higherc_align_by(bytes,4)
#define higherc_align8(bytes)                   higherc_align_by(bytes,8)
#define higherc_align16(bytes)                  higherc_align_by(bytes,16)

#endif
