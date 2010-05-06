#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "higherc/higherc.h"
#include "higherc/alloc.h"
#include "higherc/buffer.h"
#include "higherc/list.h"

/* processing buffer
 *
 *    - on top of list
 *
 *        - fixed length
 *        - fixed item size
 *
 *    - item cycling is hidden
 *    - push, flush and remaining
 *
 *        - push one
 *        - flush all
 *        - if full, remaining = 0
 *        - if full, push fails, must flush
 *
 */ 

