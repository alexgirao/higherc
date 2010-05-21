#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#define tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */

#include <stdarg.h>

#ifndef hcns /* higherc namespace */
#define hcns(v) higherc_##v
#endif

typedef int hcns(bool);   /* 0 = false, 1 = true */

#define HC_OFFSET(p, c)                ((char*)(p)+(int)(c))

#define HC_ALIGN_BY(bytes,alignment)       ((bytes + alignment) & ~(alignment-1))
#define HC_ALIGN4(bytes)                   HC_ALIGN_BY(bytes,4)
#define HC_ALIGN8(bytes)                   HC_ALIGN_BY(bytes,8)
#define HC_ALIGN16(bytes)                  HC_ALIGN_BY(bytes,16)

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define HC_FATAL(...)      hcns(fatal)(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define HC_FATAL(fmt...)   hcns(fatal)(__FILE__, __LINE__, fmt)
#endif

#define HC_MALLOC(p, sz) do { p = hcns(alloc)(sz); if ((p) == NULL) HC_FATAL("hcns(alloc)(%i)", sz); } while (0)
#define HC_NEW(p, t) do { p = (t*) hcns(alloc_z)(sizeof(t)); if ((p) == NULL) HC_FATAL("hcns(alloc_z)(%i (type " #t "))", sizeof(t)); } while (0)
#define HC_NEW_AR(p, l, t) do { p = (t*) hcns(alloc_z)((l) * sizeof(t)); if ((p) == NULL) HC_FATAL("hcns(alloc_z)(%i (%i itens of type " #t "))", (l) * sizeof(t), l); } while (0)
#define HC_FREE(p) hcns(alloc_free)(p)

struct hcns(iter);  /* forward declaration */
typedef void *(hcns(iter_next_fun))(struct hcns(iter) *);

struct hcns(iter) {
	hcns(iter_next_fun) *next;
	/* opaque data
	 */
	void *v0, *v1, *v2, *v3;
	int i0, i1, i2, i3;
	long l0, l1, l2, l3;
};

#define HC_DECL_I(name, items)						\
	struct name {							\
		int pos;						\
		struct name *tail;					\
		items							\
	};								\
	static struct name *name##_new(struct name *tail) {		\
		struct name *r;						\
		HC_NEW(r, struct name);					\
		r->pos = tail ? tail->pos + 1 : 0;			\
		r->tail = tail;						\
		return r;						\
	}								\
	static void name##_free(struct name *x)				\
	{								\
		/* ``offensive programming'' */				\
		hcns(bset)(x, sizeof(struct name), 0x7f);		\
		HC_FREE(x);						\
	}								\
	static struct name **name##_as_array(struct name *x)		\
	{								\
		struct name *t, **r;					\
		HC_NEW_AR(r, x->pos + 1, struct name*);			\
		for (t=x; t; t=t->tail) {				\
			r[t->pos] = t;					\
		}							\
		return r;						\
	}

/* this function ceases program execution with a exit(1)
 */
void hcns(fatal)(char *file, int line, char *fmt, ...);

#endif
