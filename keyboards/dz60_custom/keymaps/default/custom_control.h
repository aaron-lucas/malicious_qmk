#ifndef CUSTOM_CONTROL_H
#define CUSTOM_CONTROL_H

#include "ring_buf.h"

void send_byte_as_hex(uint8_t byte);

void open_spotlight(void);

void open_terminal(void);

void send_logged_data(ring_buf *rb);

#endif /* CUSTOM_CONTROL_H */
