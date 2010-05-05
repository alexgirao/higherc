#ifndef tid030e7a6b9335mgxvfwvibrqr0ijzf3izqrur4b9eavc /* higherc-list-h */
#define tid030e7a6b9335mgxvfwvibrqr0ijzf3izqrur4b9eavc /* higherc-list-h */

/* list header
 */
struct hcns(list) {
	int refcnt;    // reference count
	int length;    // list length
	int bufsiz;    // user data for items setup
	int lastpos;   // used to control allocated item data
	struct hcns(list) *tail;  // used for linked lists
	// list data (item + user data)
};

/* list item
 */
struct hcns(item) {
	int pos;    // data position relative to list header, 0 means undefined
	int size;   // data size
};

/* bufsz: holds item data
 * return: list object, use list_free to dispose
 */
struct hcns(list)* hcns(list_alloc)(int length, int bufsz, struct hcns(list) *tail);
void hcns(list_free)(struct hcns(list) *list);

/* index: item position at list, start at 0
 * return: pointer to user allocated area with size bytes available
 */
void *hcns(item_setup)(struct hcns(list) *list, int index, int size);

/* index: item position at list, start at 0
 * szp: pointer to a integer that will receive size, can be NULL
 * return: pointer to user allocated area or NULL if undefined (no call to item_setup)
 */
void *hcns(item_get)(struct hcns(list) *list, int index, int *sizep);

#endif
