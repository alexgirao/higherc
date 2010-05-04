#ifndef tid030e7a6b9335mgxvfwvibrqr0ijzf3izqrur4b9eavc /* higherc-list-h */
#define tid030e7a6b9335mgxvfwvibrqr0ijzf3izqrur4b9eavc /* higherc-list-h */

struct hcns(list) {
	int length;
	int bufsiz;
	struct hcns(list) *tail;
	// itens goes here
};

struct hcns(item) {
	int pos;    // position relative to list*, 0 means undefined
	int size;
	// item data goes here
};

/* bufsz: holds item data
 * return: list object, use list_free to dispose
 */
struct hcns(list)* hcns(list_alloc)(int length, int bufsz, struct hcns(list) *tail);
void hcns(list_free)(struct hcns(list) *list);

/* pos: 
 */
void hcns(list_item)(struct hcns(list) *list, int pos);

#endif
