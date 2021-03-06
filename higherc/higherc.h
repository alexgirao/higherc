#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#define tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

#include <stdarg.h>
#include <assert.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef int bool;	  /* 0 = false, 1 = true */

#define HC_TRUE 1
#define HC_FALSE 0

#define HC_OFFSET(p, c)		       ((unsigned char*)(p)+(int)(c))

#define HC_ALIGN_BY(bytes,alignment)	   ((bytes + alignment) & ~(alignment-1))
#define HC_ALIGN4(bytes)		   HC_ALIGN_BY(bytes,4)
#define HC_ALIGN8(bytes)		   HC_ALIGN_BY(bytes,8)
#define HC_ALIGN16(bytes)		   HC_ALIGN_BY(bytes,16)

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
# define HC_FATAL(...)	    _p_fatal(__FILE__, __LINE__, __VA_ARGS__)
#elif defined (__GNUC__)
# define HC_FATAL(fmt...)   _p_fatal(__FILE__, __LINE__, fmt)
#endif

/* private memory allocation functions, use macros if available
 */

void *_p_alloc(int n);
void *_p_alloc_z(int n); /* allocate with memory set to zero */
void _p_alloc_free(void *x);
void _p_alloc_fatal_error_happend(void);

/* memory allocation macros
 */

#define HC_ALLOC(p, sz) do { p = _p_alloc(sz); if ((p) == NULL) HC_FATAL("HC_ALLOC(%i)", sz); } while (0)
#define HC_ALLOC_Z(p, sz) do { p = _p_alloc_z(sz); if ((p) == NULL) HC_FATAL("HC_ALLOC_Z(%i)", sz); } while (0)
#define HC_NEW(p, t) do { p = (t*) _p_alloc_z(sizeof(t)); if ((p) == NULL) HC_FATAL("HC_NEW(%i (type " #t "))", sizeof(t)); } while (0)
#define HC_NEW_AR(p, l, t) do { p = (t*) _p_alloc_z((l) * sizeof(t)); if ((p) == NULL) HC_FATAL("HC_NEW_AR(%i (%i itens of type " #t "))", (l) * sizeof(t), l); } while (0)
#define HC_FREE(p) do { _p_alloc_free(p); p = NULL; } while (0)

/* typed list/item macros
 */

#define _HC_DECL_I_HEADERS(spec, name, st_members)			\
	struct name {							\
		int pos;						\
		struct name *tail;					\
		st_members						\
	};								\
	struct name##_iter {						\
		void *next, *end;					\
		void *v0;						\
		struct name **v1;					\
		int l0;							\
	};								\
	typedef struct name *(name##_next_func)(struct name##_iter *i);	\
	typedef void (name##_end_func)(struct name##_iter *i);		\
	spec struct name *name##_new0(struct name *tail);		\
	spec void name##_free0(struct name *x);				\
	spec struct name **name##_as_array(struct name *x);		\
	spec int name##_len(struct name *x);				\
	spec void name##_backward(struct name##_iter *i, struct name *x); \
	spec void name##_forward(struct name##_iter *i, struct name *x); \
	spec void name##_end(struct name##_iter *i);			\
	spec struct name *name##_next(struct name##_iter *i);		\
	spec struct name *name##_foreach(struct name *x[2]);		\
	spec struct name *name##_reverse(struct name *h)

#define _HC_DECL_I_IMPL(spec, name)					\
	spec struct name *name##_new0(struct name *tail) {		\
		struct name *r;						\
		HC_NEW(r, struct name);					\
		r->pos = tail ? tail->pos + 1 : 0;			\
		r->tail = tail;						\
		return r;						\
	}								\
	spec void name##_free0(struct name *x)				\
	{								\
		while (x) {						\
			struct name *t = x->tail;			\
			HC_FREE(x);					\
			x = t;						\
		}							\
	}								\
	spec struct name *name##_foreach(struct name *x[2])		\
	{								\
		if (x[1]) {						\
			x[0] = x[1];					\
			x[1] = x[0]->tail;				\
			return x[0];                                    \
		}							\
		return NULL;						\
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
	static inline struct name *_##name##_next_b(struct name##_iter *i) \
	{								\
		struct name *r = i->v0;					\
		if (r == NULL) {					\
			i->next = NULL;					\
			return NULL;					\
		}							\
		i->v0 = r->tail;					\
		return r;						\
	}								\
	spec void name##_backward(struct name##_iter *i, struct name *x) \
	{								\
		i->next = _##name##_next_b;				\
		i->end = NULL;						\
		i->v0 = x;						\
	}								\
	static inline void _##name##_end_f(struct name##_iter *i)	\
	{								\
		i->next = NULL;						\
		i->end = NULL;						\
		i->v1 = NULL;						\
		HC_FREE(i->v0);						\
	}								\
	static inline struct name *_##name##_next_f(struct name##_iter *i) \
	{								\
		if (i->l0) {						\
			i->l0--;					\
			return *i->v1++;				\
		}							\
		_##name##_end_f(i);					\
		return NULL;						\
	}								\
	spec struct name *name##_next(struct name##_iter *i)		\
	{								\
		if (i->next) {						\
			return ((name##_next_func*)i->next)(i);		\
		}							\
		return NULL;						\
	}								\
	spec void name##_end(struct name##_iter *i)			\
	{								\
		if (i->end) {						\
			((name##_end_func*)i->end)(i);			\
		}							\
	}								\
	spec void name##_forward(struct name##_iter *i, struct name *x)	\
	{								\
		i->next = _##name##_next_f;				\
		i->end = _##name##_end_f;				\
		i->v0 = name##_as_array(x);				\
		i->v1 = i->v0;						\
		i->l0 = x->pos + 1;					\
	}								\
	static inline struct name *name##_reverse0(struct name *parent, struct name *h) \
        {								\
		if (h->tail) {						\
			struct name *r;					\
			r = name##_reverse0(h, h->tail);		\
			h->tail = parent;				\
			return r;					\
		}							\
		h->tail = parent;					\
		return h;						\
	}								\
	spec struct name *name##_reverse(struct name *h)		\
	{								\
		return name##_reverse0(NULL, h);			\
	}

#define _HC_DECL_I_SORT_HEADER(spec, name, fname) spec void name##_##fname(struct name **items, int len)
#define _HC_DECL_I_USORT_HEADER(spec, name, fname) spec int name##_##fname(struct name **items, int len)

#define _HC_DECL_I_SORT_IMPL(spec, name, fname, a, b, cmpexpr)		\
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

#define _HC_DECL_I_USORT_IMPL(spec, name, fname, a, b, cmpexpr)		\
	spec int name##_##fname(struct name **items, int len)		\
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
		return len;						\
	}

/* private list item
 */

#define HC_DECL_PRIVATE_I(name, st_members)		\
	_HC_DECL_I_HEADERS(static, name, st_members);	\
	_HC_DECL_I_IMPL(static, name)

#define HC_DECL_PRIVATE_I_SORT(name, a, b, cmpexpr)			\
	_HC_DECL_I_SORT_HEADER(static, name, sort);			\
	_HC_DECL_I_SORT_HEADER(static, name, sort_desc);		\
	_HC_DECL_I_SORT_IMPL(static, name, sort, a, b, cmpexpr);	\
	_HC_DECL_I_SORT_IMPL(static, name, sort_desc, b, a, cmpexpr)

#define HC_DECL_PRIVATE_I_USORT(name, a, b, cmpexpr)			\
	_HC_DECL_I_USORT_HEADER(static, name, usort);			\
	_HC_DECL_I_USORT_HEADER(static, name, usort_desc);		\
	_HC_DECL_I_USORT_IMPL(static, name, usort, a, b, cmpexpr);	\
	_HC_DECL_I_USORT_IMPL(static, name, usort_desc, b, a, cmpexpr)

/* public list item
 */

#define HC_DECL_PUBLIC_I_HEADERS(name, st_members)	\
	_HC_DECL_I_HEADERS(, name, st_members)

#define HC_DECL_PUBLIC_I_SORT_HEADER(name)			\
	_HC_DECL_I_SORT_HEADER(, name, sort);			\
	_HC_DECL_I_SORT_HEADER(, name, sort_desc)

#define HC_DECL_PUBLIC_I_USORT_HEADER(name)		\
	_HC_DECL_I_USORT_HEADER(, name, usort);		\
	_HC_DECL_I_USORT_HEADER(, name, usort_desc)

#define HC_DECL_PUBLIC_I(name)			\
	_HC_DECL_I_IMPL(, name)

#define HC_DECL_PUBLIC_I_SORT(name, a, b, cmpexpr)		\
	_HC_DECL_I_SORT_IMPL(, name, sort, a, b, cmpexpr);	\
	_HC_DECL_I_SORT_IMPL(, name, sort_desc, b, a, cmpexpr)

#define HC_DECL_PUBLIC_I_USORT(name, a, b, cmpexpr)			\
	_HC_DECL_I_USORT_IMPL(, name, usort, a, b, cmpexpr);		\
	_HC_DECL_I_USORT_IMPL(, name, usort_desc, b, a, cmpexpr)

/* this function ceases program execution with a exit(1)
 */
void _p_fatal(char *file, int line, char *fmt, ...);

/* io functions
 */

int readfd(int fd, void *buf, int bufsz, int (*doit)(const char *buf, int len, bool eof));

/* read_exact/write_exact: return len or less/equal than 0 on error
 */
int read_exact(int fd, void *buf, int len);
int write_exact(int fd, void *buf, int len);

/* returns the value at the next 4 bytes disposed in big-endian order,
 * fatal error if can't read 4 bytes (EOF)
 */
uint32_t readfd_be4(int fd);

#endif
