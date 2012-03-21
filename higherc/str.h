#ifndef tid030d91b2551emjol04fbymqiis5x2xpeoswch30w2g6 /* higherc-str-h */
#define tid030d91b2551emjol04fbymqiis5x2xpeoswch30w2g6 /* higherc-str-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

int scopy(char *s, const char *t); /* copy t into s, til '\0' */
int sdiff(const char *s, const char *t); /* returns s - t, 0 if equal */
int sdiffn(const char *s, const char *t, int len);
int slen(const char *s);
int schrl(const char *s, int c); /* search c from left */
int schrr(const char *s, int c); /*  search c from right */
bool sstart(const char *s, const char *t);  /* t is substring of s?*/

#endif
