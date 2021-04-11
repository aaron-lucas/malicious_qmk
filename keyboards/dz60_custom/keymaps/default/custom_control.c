#include QMK_KEYBOARD_H

#include "custom_control.h"
#include "key_event.h"

const char TERMINAL[] PROGMEM = "terminal";

void send_byte_as_hex(uint8_t byte) {
    static const char HEX[16] = "0123456789ABCDEF";

    char hi = HEX[(byte >> 4) & 0xF];
    char lo = HEX[byte & 0xF];

    send_char(hi);
    send_char(lo);
}

void open_spotlight(void) {
    register_code(KC_LGUI);
    register_code(KC_SPACE);
    unregister_code(KC_SPACE);
    unregister_code(KC_LGUI);
}

void open_terminal(void) {
    send_string_P(TERMINAL);
    tap_code(KC_ENT);
}

void send_logged_data(ring_buf *rb) {
    for (uint16_t i = 0; i < rb->length; i++) {
        struct key_event event = rb_peek_at(rb, i);
        uint8_t byte = EVENT_TO_BYTE(event);
        send_byte_as_hex(byte);
    }
}

