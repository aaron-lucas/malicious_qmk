#include "ring_buf.h"

void rb_push_back(ring_buf *rb, struct key_event event) {
    if (rb->length < BUF_LEN) {
        uint16_t slot = (rb->head + rb->length) % BUF_LEN;
        rb->events[slot] = event;
        rb->length++;
    } else {
        rb->events[rb->head] = event;
        rb->head = (rb->head + 1) % BUF_LEN;
    }

}

struct key_event rb_pop_front(ring_buf *rb) {
    if (rb->length <= 0) {
        return NO_EVENT;
    }

    struct key_event event = rb->events[rb->head];

    if (rb->head == BUF_LEN - 1) {
        rb->head = 0;
    } else {
        rb->head++;
    }

    rb->length--;

    return event;
}

struct key_event rb_peek_at(ring_buf *rb, uint16_t index) {
    if (index < 0 || index >= rb->length) {
        return NO_EVENT;
    }

    uint16_t rb_idx = (rb->head + index) % BUF_LEN;

    return rb->events[rb_idx];
}

