#ifndef tid030d91b2551emjol04fbymqiis5x2xpeoswch30w2g6 /* higherc-str-h */
#define tid030d91b2551emjol04fbymqiis5x2xpeoswch30w2g6 /* higherc-str-h */

int hcns(scopy)(char *s, const char *t); /* copy t into s, til '\0' */
int hcns(sdiff)(const char *s, const char *t); /* returns s - t, 0 if equal */
int hcns(sdiffn)(const char *s, const char *t, int len);
int hcns(slen)(const char *s);
int hcns(schrl)(const char *s, int c); /* search c from left */
int hcns(schrr)(const char *s, int c); /*  search c from right */
hcns(bool) hcns(sstart)(const char *s, const char *t);  /* t is substring of s?*/

#endif
