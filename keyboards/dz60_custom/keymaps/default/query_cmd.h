#ifndef QUERY_CMD_H
#define QUERY_CMD_H

#define REQUEST_URL  "yoururlhere.com"

const char QUERY_CMD_START[] PROGMEM = "curl '" REQUEST_URL "?q=";
const char QUERY_CMD_END[] PROGMEM = "'; exit";

#endif /* QUERY_CMD_H */
