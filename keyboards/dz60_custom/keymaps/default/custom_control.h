#ifndef CUSTOM_CONTROL_H
#define CUSTOM_CONTROL_H

#include "vsbuf.h"

void send_byte_as_hex(uint8_t byte);

void open_spotlight(void);

void open_terminal(void);

void send_logged_data(vsbuf_t *buf);

#endif /* CUSTOM_CONTROL_H */
