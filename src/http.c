#include <stdio.h>

#include "http.h"


int serialize_http_response(struct response* res, char* buf, size_t len) {
    size_t total = 0;
    size_t maxlen = len + 1; // Length without '\0'
    // Status line
    int bytes = snprintf(buf, maxlen, "%s %d %s\r\n", res->version, res->code, res->phrase);
    if (bytes < 0 || bytes > maxlen) return -1;
    total += bytes;
    // Headers
    for (int i = 0; i < res->content.headers_n; i++) {
        bytes = snprintf(buf+total, maxlen-total, "%s: %s\r\n", res->content.headers[i].name, res->content.headers[i].value);
        if (bytes < 0 || bytes > maxlen-total) return -1;
        total += bytes;
    }
    // \r\n
    bytes = snprintf(buf+total, maxlen-total, "\r\n");
    if (bytes < 0 || bytes > maxlen-total) return -1;
    total += bytes;

    return total;
}

int deserialize_http_request(char* buf, size_t len, struct request* req) {
    
}