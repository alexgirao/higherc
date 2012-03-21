#ifndef tid030e7a6b9335mgxvfwvibrqr0ijzf3izqrur4b9eavc /* higherc-list-h */
#define tid030e7a6b9335mgxvfwvibrqr0ijzf3izqrur4b9eavc /* higherc-list-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

/* list: a mixture of linked list and array intended for heavy, fast,
 * flexible list processing
 */

/* todo:
 *
 *	list alloc with tail (incref on all lists)
 *	list free with tail (decref on all lists)
 *	list traverse with tail example
 *	list reverse
 *	list duplicate
 *	list flatten
 */

/* list header
 */
struct list {
	int refcnt;    // reference count
	int length;    // list length
	int bufsiz;    // user data for items setup
	int lastpos;   // used to control allocated item data
	struct list *tail;  // used for linked lists
	// list data (item + user data)
};

/* list item
 */
struct item {
	int pos;    // data position relative to list header, 0 means undefined
	int size;   // data size
};

/* bufsz: holds item data
 * return: list object, use list_free to dispose
 */
struct list* list_alloc(int length, int bufsz, struct list *tail);

/* return: false (0) if reference count less than or equal zero
 */
void list_free(struct list *list);

/*
 * index: item position at list, start at 0
 *
 * return: pointer to memory with `size' bytes, MUST NOT be freed,
 *   will be freed automatically with list. NULL if called more than
 *   once or if called with invalid arguments
 */
void *item_setup(struct list *list, int index, int size);

/* index: item position at list, start at 0
 * szp: pointer to a integer that will receive size, can be NULL
 * return: pointer to user allocated area or NULL if undefined (no call to item_setup)
 */
void *item_get(struct list *list, int index, int *sizep);

#endif
