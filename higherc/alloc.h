#ifndef tid030f776035aaoxxcc9dyo23upvumyslamfvqvtt0iu7 /* higherc-alloc-h */
#define tid030f776035aaoxxcc9dyo23upvumyslamfvqvtt0iu7 /* higherc-alloc-h */

void *hcns(alloc)(int n);
void *hcns(alloc_z)(int n); /* allocate with memory set to zero */
void hcns(alloc_free)(void *x);
void *hcns(alloc_re)(void *x, int m, int n); /* reallocate n copying only m bytes */
int hcns(alloc_delta)(void);
void hcns(alloc_fatal_error_happend)(void);

#endif
