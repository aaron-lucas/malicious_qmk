#ifndef QUERY_CMD_H
#define QUERY_CMD_H

#include "request_url.h"

const char QUERY_CMD_START[] PROGMEM = "curl '" REQUEST_URL "?q=";
const char QUERY_CMD_END[] PROGMEM = "'; exit";

#endif /* QUERY_CMD_H */
