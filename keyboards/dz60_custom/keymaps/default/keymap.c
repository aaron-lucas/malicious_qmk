#include QMK_KEYBOARD_H

enum custom_keycodes {
    TEST = SAFE_RANGE
};

#define QUEUE_LEN 128

struct key_event {
    uint16_t keycode;
    keyrecord_t record;
};

struct queue {
    struct key_event data[QUEUE_LEN];
    uint16_t head;
    uint16_t length;
};

static void enqueue(struct queue *q, struct key_event *item) {
    struct key_event copy = *item;

    if (q->length < QUEUE_LEN) {
        uint16_t slot = (q->head + q->length) % QUEUE_LEN;
        q->data[slot] = copy;
        q->length++;
    } else {
        q->data[q->head] = copy;
        q->head = (q->head + 1) % QUEUE_LEN;
    }

}

/* static struct key_event dequeue(struct queue *q) { */
/*     if (q->length > 0) { */
/*         struct key_event item = q->data[q->head]; */

/*         if (q->head == QUEUE_LEN - 1) { */
/*             q->head = 0; */
/*         } else { */
/*             q->head++; */
/*         } */

/*         q->length--; */

/*         return item; */
/*     } */

/*     return (struct key_event){ 0 }; */
/* } */

struct queue keylog = { 0 };


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case TEST:
        if (record->event.pressed) {
            for (uint16_t i = 0; i < keylog.length; i++) {
                struct key_event event = keylog.data[i];
                if (event.record.event.pressed) {
                    register_code16(event.keycode);
                } else {
                    unregister_code16(event.keycode);
                }
            }
        } else {
            // Key released
        }
        return false;
    default: ;
        struct key_event event = { .keycode = keycode, .record = *record };
        enqueue(&keylog, &event);
        break;
    }

    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT(
        KC_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  XXXXXXX, KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, XXXXXXX, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, XXXXXXX,
        KC_LCTL, KC_LALT, KC_LGUI,                   KC_SPC,  KC_SPC,  KC_SPC,           KC_RGUI, KC_RALT, XXXXXXX, MO(1),   TEST
    ),

    LAYOUT(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, KC_DEL,
        _______, RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______, _______, _______, _______, RESET,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, BL_DEC,  BL_TOGG, BL_INC,  BL_STEP, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                   _______, _______, _______,          _______, _______, _______, _______, _______
    )
};
