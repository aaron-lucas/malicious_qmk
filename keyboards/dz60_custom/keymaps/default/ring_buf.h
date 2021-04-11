#ifndef RING_BUF_H
#define RING_BUF_H

#include "key_event.h"

#define BUF_LEN 2048
typedef struct {
    struct key_event events[BUF_LEN];
    uint16_t head;
    uint16_t length;
} ring_buf;

void rb_push_back(ring_buf *rb, struct key_event event);

struct key_event rb_pop_front(ring_buf *rb);

struct key_event rb_peek_at(ring_buf *rb, uint16_t index);

#endif /* RING_BUF */
