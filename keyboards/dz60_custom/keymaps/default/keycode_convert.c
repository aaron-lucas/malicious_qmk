#include "keycode_convert.h"

uint8_t map_qmk_kc(uint16_t qmk_kc) {
    if (qmk_kc >= KC_A && qmk_kc <= KC_KP_EQUAL) {
        return (uint8_t)qmk_kc;
    } else if (qmk_kc >= KC_LCTRL && qmk_kc <= KC_RGUI) {
        return (uint8_t)(qmk_kc - MOD_OFFSET);
    }

    return NO_KEY_MAPPING;
}

uint16_t get_qmk_kc(uint8_t mapped_kc) {
    if (mapped_kc >= KC_A && mapped_kc <= KC_KP_EQUAL) {
        return (uint16_t)mapped_kc;
    } else if (mapped_kc + MOD_OFFSET <= KC_RGUI) {
        return (uint16_t)(mapped_kc + MOD_OFFSET);
    }

    return NO_KEY_MAPPING;
}
