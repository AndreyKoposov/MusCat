#include <stdio.h>
#include <string.h>

#include "http.h"


int add_header(struct content* c, char* name, char* value) {
    int i = c->headers_n;
    if (i >= MAX_HEADERS_N) 
        return -1;
    c->headers_n++;

    strncpy(c->headers[i].name, name, MAX_NAME_L);
    strncpy(c->headers[i].value, value, MAX_VAL_L);

    return 0;
}

int add_date(struct content* c) {

}

int add_length(struct content* c, size_t len) {
    char value[MAX_VAL_L+2+1]; // \r\n + \0
    int bytes = snprintf(value, MAX_VAL_L, "%d\r\n", len);
    if (bytes < 0 || bytes > MAX_VAL_L) return -1;

    c->body_len = len;
    
    return add_header(c, "Content-Length: ", value);
}

int add_body(struct content* c, char* body) {

}

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