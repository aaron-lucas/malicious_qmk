#include "vsbuf.h"

#define COLLIDED(buf) ((buf)->v_end == ((buf)->s_end - 1))

void vsbuf_reset(vsbuf_t *buf) {
    // Actual data need not be changed as resetting the indexes will effectively
    // prevent the old data from being used
    buf->v_lru = 0;
    buf->v_end = V_END_EMPTY;

    buf->s_end = VSBUF_SIZE;
}

void vsbuf_add_volatile(vsbuf_t *buf, struct key_event event) {
    if (COLLIDED(buf)) {
        // Volatile and saved sections have collided so we must replace an item
        // in the volatile section as the saved section cannot be made smaller.
        // The strategy used is to replace the oldest entry in the volatile
        // section.
        buf->events[buf->v_lru] = event;

        // Volatile section starts acting as a ring buffer
        if (buf->v_lru == buf->v_end) {
            buf->v_lru = 0;
        } else {
            buf->v_lru++;
        }
    } else {
        // Volatile and saved sections have not collided so we can add an item
        // without replacement. It is also known that v_lru = 0.
        if (buf->v_end == V_END_EMPTY) {
            buf->events[0] = event;
            buf->v_end = 0;
        } else {
            buf->events[++(buf->v_end)] = event;
        }
    }
}

uint8_t vsbuf_add_saved(vsbuf_t *buf, struct key_event event) {
    if (buf->s_end <= SAVE_MAX) {
        // Reached the upper limit of number of saved key events
        return ERR_SAVED_FULL;
    } 

    if (COLLIDED(buf)) {
        // Delete the least recently used event and shift all events between
        // that and v_end down to make space for the entry in the saved section
        if (buf->v_lru == buf->v_end) {
            // Edge case when the LRU item is at the end of the saved section.
            // Nothing needs to be shifted and this item will be overwritten
            // later.
            buf->v_lru = 0;
        } else {
            for (uint16_t i = buf->v_lru; i < buf->v_end; i++) {
                buf->events[i] = buf->events[i + 1];
            }
        }
        
        // Reduce the size of the volatile section
        buf->v_end--;
    }

    // Add item to saved section
    buf->events[--(buf->s_end)] = event;

    return 0;
}

struct key_event vsbuf_get_volatile(vsbuf_t *buf, uint16_t index) {
    if (index > buf->v_end) {
        return NO_EVENT;
    }

    // Index 0 is the least recently used item
    uint16_t relative_idx = (buf->v_lru + index) % (buf->v_end + 1);
    return buf->events[relative_idx];
}

struct key_event vsbuf_get_saved(vsbuf_t *buf, uint16_t index) {
    if (index >= VSBUF_SIZE - buf->s_end) {
        return NO_EVENT;
    }

    // Saved events are never replaced so index 0 always corresponds to the top
    // of the saved section.
    return buf->events[VSBUF_SIZE - 1 - index];
}

uint16_t vsbuf_volatile_size(vsbuf_t *buf) {
    return buf->v_end + 1;
}

uint16_t vsbuf_saved_size(vsbuf_t *buf) {
    return VSBUF_SIZE - buf->s_end;
}
