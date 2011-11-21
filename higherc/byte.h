#ifndef tid030e7d411279fs42u4err9cv1o9g9gwl992ba0mr6x1 /* higherc-byte-h */
#define tid030e7d411279fs42u4err9cv1o9g9gwl992ba0mr6x1 /* higherc-byte-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

int hcns(b_chrl)(const void *s, int n, int c);
int hcns(b_chrr)(const void *s, int n, int c);

/* copy from left to right, use it if shifting from left to right on
 * overlapping buffers
 */
void hcns(b_copyl)(void *to, int n, const void *from);

/* copy from right to left, use it if shifting from right to left on
 * overlapping buffers
 */
void hcns(b_copyr)(void *to, int n, const void *from);

/* use bcopyl or bcopyr automatically
 */
void hcns(b_copy)(void *to, int n, const void *from);

int hcns(b_diff)(const void *s, int n, const void *t);
void hcns(b_zero)(void *s, int n);
void hcns(b_rev)(void *s, int n);
void hcns(b_set)(void *s, int n, char c);

#endif
