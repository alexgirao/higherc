#ifndef tid030fcf561884hn35vz11nhs6s9yw18g31tdeosz4q25 /* higherc-fatal-h */
#define tid030fcf561884hn35vz11nhs6s9yw18g31tdeosz4q25 /* higherc-fatal-h */

#include <stdarg.h>

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define HC_FATAL(...)      hcns(fatal)(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define HC_FATAL(fmt...)   hcns(fatal)(__FILE__, __LINE__, fmt)
#endif

/* this function ceases program execution with a exit(1)
 */
void hcns(fatal)(char *file, int line, char *fmt, ...);

/*
 */

#define HC_MALLOC(p, sz) do { p = hcns(alloc)(sz); if ((p) == NULL) FATAL("hcns(alloc)(%i)", sz); } while (0)
#define HC_NEW(p, t) do { p = (t*) hcns(alloc_z)(sizeof(t)); if ((p) == NULL) FATAL("hcns(alloc_z)(%i (type " #t "))", sizeof(t)); } while (0)
#define HC_NEW_AR(p, l, t) do { p = (t*) hcns(alloc_z)(l * sizeof(t)); if ((p) == NULL) FATAL("hcns(alloc_z)(%i (%i itens of type " #t "))", l * sizeof(t), l); } while (0)
#define HC_FREE(p) hcns(alloc_free)(p)

#endif
