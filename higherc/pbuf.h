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
	int enqueued; /* how many items enqueued, decreasing by N has
			 the effect of dequeueing N items
			 immediately */
};

#define HC_NULL_PBUF {NULL, 0, 0, 0}
#define HC_ST_PBUF struct hcns(pbuf)
#define HC_DEF_PBUF(sym) HC_ST_PBUF sym[1] = {HC_NULL_PBUF}

void hcns(pbuf_alloc)(struct hcns(pbuf) *pbuf, int len, int itemsz);
void hcns(pbuf_free)(struct hcns(pbuf) *pbuf);

/* get remaining slots at cyclic queue, before dequeue is required
 */
int hcns(remaining)(struct hcns(pbuf) *pbuf);

/* get item for write
 * return: NULL on error or if queue is full
 */
void *hcns(enqueue)(struct hcns(pbuf) *pbuf);

/* get last enqueued item for read
 * return: NULL on error or if queue is empty
 */
void *hcns(shift)(struct hcns(pbuf) *pbuf);

/* get first enqueued item for read
 * return: NULL on error or if queue is empty
 */
void *hcns(dequeue)(struct hcns(pbuf) *pbuf);

#endif
