#ifndef tid030ec912423d5pihgyag2qd0gxzwhkh34gy5o8oj089 /* higherc-pbuf-h */
#define tid030ec912423d5pihgyag2qd0gxzwhkh34gy5o8oj089 /* higherc-pbuf-h */

#ifndef tid0209a64d4a69ik72qqii2sl0nhqrza49rb6ltavx6ug /* higherc-higherc-h */
#error higherc-h not included
#endif

/* processing buffer, cyclic queue
 *
 *    - data collection for later batch processing (flush all enqueued items)
 *    - on top of list
 *
 *        - queue length (user-defined, non-dynamic)
 *        - queue item size (user-defined, non-dynamic)
 *
 *    - enqueue, dequeue and remaining
 *
 *        - enqueue item
 *        - dequeue item
 *        - if empty, remaining = queue length
 *        - if full, remaining = 0
 *        - if full, enqueue fails, must dequeue
 *
 */ 

struct pbuf {
	struct list *list;
	int itemsiz;
	int next; /* next available item */
	int enqueued; /* how many items enqueued, decreasing by N has
			 the effect of dequeueing N items
			 immediately */
};

#define HC_NULL_PBUF {NULL, 0, 0, 0}
#define HC_ST_PBUF struct pbuf
#define HC_DEF_PBUF(sym) HC_ST_PBUF sym[1] = {HC_NULL_PBUF}

void pbuf_alloc(struct pbuf *pbuf, int len, int itemsz);
void pbuf_free(struct pbuf *pbuf);

/* get item for write
 * return: NULL on error or if queue is full
 */
void *pbuf_enqueue(struct pbuf *pbuf);

/* get last enqueued item for read
 * return: NULL on error or if queue is empty
 */
void *pbuf_shift(struct pbuf *pbuf);

/* get first enqueued item for read
 * return: NULL on error or if queue is empty
 */
void *pbuf_dequeue(struct pbuf *pbuf);

/* get first remaining item
 */
int pbuf_enqueued_first(struct pbuf *pbuf);
int pbuf_enqueued_last(struct pbuf *pbuf);
int pbuf_enqueued_len(struct pbuf *pbuf);

int pbuf_remaining_first(struct pbuf *pbuf);
int pbuf_remaining_last(struct pbuf *pbuf);
int pbuf_remaining_len(struct pbuf *pbuf);

/* pbuf's length
 */
int pbuf_len(struct pbuf *pbuf);

int pbuf_next(struct pbuf *pbuf, int pos);
int pbuf_prior(struct pbuf *pbuf, int pos);

/* get item at slot, queued or not
 */
void * pbuf_item(struct pbuf *pbuf, int pos);

#endif
