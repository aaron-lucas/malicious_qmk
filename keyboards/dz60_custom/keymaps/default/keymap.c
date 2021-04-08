#include QMK_KEYBOARD_H

#include "print.h"

enum custom_keycodes {
    DUMP = SAFE_RANGE,
    TEST
};

struct key_event {
    uint8_t is_pressed : 1;
    uint8_t mapped_kc  : 7;     // Keycode is encoded to fit within 7 bits. QMK has 8-bit keycodes
};

#define NO_EVENT            ((struct key_event){ .is_pressed = 0, .mapped_kc = 0 })
#define NO_KEY_MAPPING      0

#define BUF_LEN 2048
typedef struct _ring_buf {
    struct key_event events[BUF_LEN];
    uint16_t head;
    uint16_t length;
} ring_buf;

static void rb_push_back(ring_buf *rb, struct key_event event) {
    if (rb->length < BUF_LEN) {
        uint16_t slot = (rb->head + rb->length) % BUF_LEN;
        rb->events[slot] = event;
        rb->length++;
    } else {
        rb->events[rb->head] = event;
        rb->head = (rb->head + 1) % BUF_LEN;
    }

}

/* static struct key_event rb_pop_front(ring_buf *rb) { */
/*     if (rb->length <= 0) { */
/*         return NO_EVENT; */
/*     } */

/*     struct key_event event = rb->events[rb->head]; */

/*     if (rb->head == BUF_LEN - 1) { */
/*         q->head = 0; */
/*     } else { */
/*         q->head++; */
/*     } */

/*     q->length--; */

/*     return event; */
/* } */

static struct key_event rb_peek_at(ring_buf *rb, uint16_t index) {
    if (index < 0 || index >= rb->length) {
        return NO_EVENT;
    }

    uint16_t rb_idx = (rb->head + index) % BUF_LEN;

    /* dprintf("peeking at index %d\n", rb_idx); */

    return rb->events[rb_idx];
}

#define MOD_OFFSET (KC_LCTRL - (KC_KP_EQUAL + 1))
static uint8_t map_qmk_kc(uint16_t qmk_kc) {
    if (qmk_kc >= KC_A && qmk_kc <= KC_KP_EQUAL) {
        return (uint8_t)qmk_kc;
    } else if (qmk_kc >= KC_LCTRL && qmk_kc <= KC_RGUI) {
        return (uint8_t)(qmk_kc - MOD_OFFSET);
    }

    return NO_KEY_MAPPING;
}

static uint16_t get_qmk_kc(uint8_t mapped_kc) {
    if (mapped_kc >= KC_A && mapped_kc <= KC_KP_EQUAL) {
        return (uint16_t)mapped_kc;
    } else if (mapped_kc + MOD_OFFSET <= KC_RGUI) {
        return (uint16_t)(mapped_kc + MOD_OFFSET);
    }

    return NO_KEY_MAPPING;
}



ring_buf keylog = { .head = 0, .length = 0 };


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* dprintf("keycode: 0x%04X\n", keycode); */

    uint8_t mapped_kc = map_qmk_kc(keycode);
    if (mapped_kc != NO_KEY_MAPPING) {
        struct key_event event = {
            .is_pressed = record->event.pressed,
            .mapped_kc = mapped_kc
        };

        rb_push_back(&keylog, event);
    }

    switch (keycode) {
    case DUMP:
        if (record->event.pressed) {
            for (uint16_t i = 0; i < keylog.length; i++) {
                struct key_event event = rb_peek_at(&keylog, i);

                uint16_t qmk_kc = get_qmk_kc(event.mapped_kc);

                if (event.is_pressed) {
                    register_code16(qmk_kc);
                } else {
                    unregister_code16(qmk_kc);
                }
            }
        } else {
            // Key released
        }
        return false;
    case TEST:
        if (record->event.pressed)
            register_code16(0x0204); // 'a' with left shift
        else
            unregister_code16(0x0204);

        break;
   }

    return true;
}

void keyboard_post_init_user(void) {
    debug_enable = true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT(
        KC_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  XXXXXXX, KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, XXXXXXX, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, XXXXXXX,
        KC_LCTL, RESET,   KC_LGUI,                   KC_SPC,  KC_SPC,  KC_SPC,           KC_RGUI, TEST   , XXXXXXX, MO(1),   DUMP
    ),

    LAYOUT(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_DEL,
        _______, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______, _______, RESET,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, BL_DEC,  BL_TOGG, BL_INC,  BL_STEP, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                   _______, _______, _______,          _______, _______, _______, _______, _______
    )
};
