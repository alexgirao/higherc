#ifndef tid030d91b2551emjol04fbymqiis5x2xpeoswch30w2g6 /* higherc-str-h */
#define tid030d91b2551emjol04fbymqiis5x2xpeoswch30w2g6 /* higherc-str-h */

int hcns(str_copy)(char *s, const char *t); /* copy t into s, til '\0' */
int hcns(str_diff)(const char *s, const char *t); /* returns s - t, 0 if equal */
int hcns(str_diffn)(const char *s, const char *t, int len);
int hcns(str_len)(const char *s);
int hcns(str_chrl)(const char *s, int c); /* search c from left */
int hcns(str_chrr)(const char *s, int c); /*  search c from right */
int hcns(str_start)(const char *s, const char *t);  /* t is substring of s?*/

#endif
