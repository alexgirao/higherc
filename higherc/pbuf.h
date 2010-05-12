#ifndef tid030ec912423d5pihgyag2qd0gxzwhkh34gy5o8oj089 /* higherc-pbuf-h */
#define tid030ec912423d5pihgyag2qd0gxzwhkh34gy5o8oj089 /* higherc-pbuf-h */

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

struct hcns(pbuf) {
	struct hcns(list) *list;
	int itemsiz;
	int next; /* next available item */
	int enqueued;  /* how many items enqueued */
};

#define HC_NULL_PBUF {NULL, 0, 0, 0}

hcns(bool) hcns(pbuf_alloc)(struct hcns(pbuf) *pbuf, int length, int itemsz);
hcns(bool) hcns(pbuf_free)(struct hcns(pbuf) *pbuf);

/* get remaining slots at cyclic queue, before dequeue is required
 */
int hcns(remaining)(struct hcns(pbuf) *pbuf);

/* get item for write
 * return: NULL on error or if queue is full
 */
void *hcns(enqueue)(struct hcns(pbuf) *pbuf);

/* get item for read
 * return: NULL on error or if queue is empty
 */
void *hcns(dequeue)(struct hcns(pbuf) *pbuf);

#endif
