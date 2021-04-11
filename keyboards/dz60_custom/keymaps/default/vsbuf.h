#ifndef VSBUF_H
#define VSBUF_H

#include "key_event.h"

#define ERR_SAVED_FULL  1

#define VSBUF_SIZE      2048
#define V_END_EMPTY     (VSBUF_SIZE + 1)
#define SAVE_MAX        512
typedef struct vsbuf {
    struct key_event events[VSBUF_SIZE];
    uint16_t v_lru;     // Index of the least recently used volatile entry
    uint16_t v_end;     // Largest absolute index of the volatile section
    uint16_t s_end;     // Smallest absolute index of the saved section
} vsbuf_t;

void vsbuf_reset(vsbuf_t *buf);

void vsbuf_add_volatile(vsbuf_t *buf, struct key_event event);

uint8_t vsbuf_add_saved(vsbuf_t *buf, struct key_event event);

struct key_event vsbuf_get_volatile(vsbuf_t *buf, uint16_t index);

struct key_event vsbuf_get_saved(vsbuf_t *buf, uint16_t index);

uint16_t vsbuf_volatile_size(vsbuf_t *buf);

uint16_t vsbuf_saved_size(vsbuf_t *buf);

#endif /* VSBUF_H */
