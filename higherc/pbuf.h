#ifndef tid030ec912423d5pihgyag2qd0gxzwhkh34gy5o8oj089 /* higherc-pbuf-h */
#define tid030ec912423d5pihgyag2qd0gxzwhkh34gy5o8oj089 /* higherc-pbuf-h */

/* processing buffer
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

#endif
