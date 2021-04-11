#ifndef KEYCODE_CONVERT_H
#define KEYCODE_CONVERT_H

#include QMK_KEYBOARD_H

#define MOD_OFFSET (KC_LCTRL - (KC_KP_EQUAL + 1))
#define NO_KEY_MAPPING          0

uint8_t map_qmk_kc(uint16_t qmk_kc);

uint16_t get_qmk_kc(uint8_t mapped_kc);

#endif /* KEYCODE_CONVERT_H */
