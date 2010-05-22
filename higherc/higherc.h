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
#define HC_FREE(p) do { hcns(alloc_free)(p); p = NULL; } while (0)

#define _HC_DECL_I_HEADERS(spec, name, st_members)			\
	struct name {							\
		int pos;						\
		struct name *tail;					\
		st_members						\
	};								\
	struct name##_iter;  /* forward declaration */			\
	typedef struct name *(name##_next_fun)(struct name##_iter *);	\
	struct name##_iter {						\
		name##_next_fun *next;					\
		int i0;							\
		void *v0, *v1;						\
		long l0;						\
	};								\
	spec struct name *name##_new(struct name *tail);		\
	spec void name##_free(struct name *x);				\
	spec struct name **name##_as_array(struct name *x);		\
	spec int name##_len(struct name *x);				\
	spec void name##_backward(struct name *x, struct name##_iter *i); \
	spec void name##_forward(struct name *x, struct name##_iter *i); \
	spec void name##_end(struct name##_iter *i)

#define _HC_DECL_I(spec, name)						\
	spec struct name *name##_new(struct name *tail) {		\
		struct name *r;						\
		HC_NEW(r, struct name);					\
		r->pos = tail ? tail->pos + 1 : 0;			\
		r->tail = tail;						\
		return r;						\
	}								\
	spec void name##_free(struct name *x)				\
	{								\
		HC_FREE(x);						\
	}								\
	spec struct name **name##_as_array(struct name *x)		\
	{								\
		struct name *t, **r;					\
		HC_NEW_AR(r, x->pos + 1, struct name*);			\
		for (t=x; t; t=t->tail) {				\
			r[t->pos] = t;					\
		}							\
		return r;						\
	}								\
	spec int name##_len(struct name *x)				\
	{								\
		return x->pos + 1;					\
	}								\
	static inline struct name *_name##_next_b(struct name##_iter *i) \
	{								\
		struct name *r = i->v0;					\
		if (r == NULL) {					\
			return NULL;					\
		}							\
		i->v0 = r->tail;					\
		return r;						\
	}								\
	spec void name##_backward(struct name *x, struct name##_iter *i) \
	{								\
		i->next = _name##_next_b;				\
		i->i0 = 1;						\
		i->v0 = x;						\
	}								\
	static inline struct name *_name##_next_f(struct name##_iter *i) \
	{								\
		struct name **r = i->v1;				\
		if (r == NULL) {					\
			return NULL;					\
		}							\
		if (i->l0-- == 0) {					\
			HC_FREE(i->v0);					\
			i->v0 = NULL;					\
			i->v1 = NULL;					\
			return NULL;					\
		}							\
		i->v1 = r + 1;						\
		return *r;						\
	}								\
	spec void name##_forward(struct name *x, struct name##_iter *i)	\
	{								\
		i->next = _name##_next_f;				\
		i->i0 = 2;						\
		i->v0 = name##_as_array(x);				\
		i->v1 = i->v0;						\
		i->l0 = x->pos + 1;					\
	}								\
	spec void name##_end(struct name##_iter *i)			\
	{								\
		if (i->i0 == 2) {					\
			if (i->v0) {					\
				HC_FREE(i->v0);				\
				i->v0 = NULL;				\
			}						\
		} else if (i->i0 == 1) {				\
		} else {						\
			HC_FATAL("exhausted");				\
		}							\
	}

#define _HC_DECL_I_SORT_HEADER(spec, name, fname) spec void name##_##fname(struct name **items, int len);
#define _HC_DECL_I_USORT_HEADER(spec, name, fname) spec void name##_##fname(struct name **items, int len, int *newlen);

#define _HC_DECL_I_SORT(spec, name, fname, a, b, cmpexpr)		\
	spec void name##_##fname(struct name **items, int len)		\
	{								\
		int i, r;						\
		struct name *a, *b;					\
		for(i=1; i<len; i++) {					\
			int j = i - 1;					\
			b = items[i];					\
			while (j>=0) {					\
				a = items[j];				\
				r = cmpexpr;				\
				if (r > 0) {				\
					/* shift down elements greater than \
					 * input (b)			\
					 */				\
					items[j+1] = items[j];		\
					j--;				\
				} else {				\
					break;				\
				}					\
			}						\
			items[j+1] = b;					\
		}							\
	}

#define _HC_DECL_I_USORT(spec, name, fname, a, b, cmpexpr)		\
	spec void name##_##fname(struct name **items, int len, int *newlen) \
	{								\
		int i, r;						\
		struct name *a, *b;					\
		for(i=1; i<len; i++) {					\
			int j = i - 1;					\
			int k = j;					\
			int dup_found = 0;				\
			b = items[i];					\
			while (j>=0) {					\
				a = items[j];				\
				r = cmpexpr;				\
				if (r > 0) {				\
					j--;				\
				} else if (r == 0) {			\
					dup_found = 1;			\
					break;				\
				} else {				\
					break;				\
				}					\
			}						\
			if (dup_found) {				\
				/* discard duplicate shifting up unprocessed \
				 * elements				\
				 */					\
				k = i + 1;				\
				while (k < len) {			\
					items[k - 1] = items[k];	\
					k++;				\
				}					\
				len--;					\
				i--; /* reprocess discarded slot */	\
			} else {					\
				while (k != j) {			\
					/* shift down elements greater than \
					 * input (b)			\
					 */				\
					items[k + 1] = items[k];	\
					k--;				\
				}					\
				items[k + 1] = b;			\
			}						\
		}							\
		*newlen = len;						\
	}

/* private list item
 */

#define HC_DECL_PRIVATE_I(name, st_members)		\
	_HC_DECL_I_HEADERS(static, name, st_members);	\
	_HC_DECL_I(static, name)

#define HC_DECL_PRIVATE_I_SORT(sym, a, b, cmpexpr)		\
	_HC_DECL_I_SORT_HEADER(static, sym, sort);		\
	_HC_DECL_I_SORT_HEADER(static, sym, sort_desc);		\
	_HC_DECL_I_SORT(static, sym, sort, a, b, cmpexpr)	\
	_HC_DECL_I_SORT(static, sym, sort_desc, b, a, cmpexpr)

#define HC_DECL_PRIVATE_I_USORT(sym, a, b, cmpexpr)			\
	_HC_DECL_I_USORT_HEADER(static, sym, usort);			\
	_HC_DECL_I_USORT_HEADER(static, sym, usort_desc);		\
	_HC_DECL_I_USORT(static, sym, usort, a, b, cmpexpr)		\
	_HC_DECL_I_USORT(static, sym, usort_desc, b, a, cmpexpr)

/* public list item
 */

#define HC_DECL_PUBLIC_I_HEADERS(name, st_members)	\
	_HC_DECL_I_HEADERS(, name, st_members);
#define HC_DECL_PUBLIC_I(name)			\
	_HC_DECL_I(, name)

/* this function ceases program execution with a exit(1)
 */
void hcns(fatal)(char *file, int line, char *fmt, ...);

#endif
