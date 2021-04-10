#ifndef KEY_EVENT_H
#define KEY_EVENT_H

struct key_event{
    // Keycode is encoded to fit within 7 bits. QMK has 8-bit keycodes
    uint8_t mapped_kc  : 7; 
    uint8_t is_pressed : 1;
};

#define NO_EVENT                (struct key_event){ 0, 0 }

// Shift constants for bitfields
#define EVENT_KEYCODE_S         0
#define EVENT_PRESSED_S         7
#define EVENT_TO_BYTE(event)    (((event.is_pressed) << EVENT_PRESSED_S) | \
                                 ((event.mapped_kc)  << EVENT_KEYCODE_S))

#endif /* KEY_EVENT_H */
