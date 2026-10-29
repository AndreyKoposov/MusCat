#include <stdio.h>
#include <string.h>

#include "http.h"


int add_status_line_response(struct response* res, char* ver, int code, char* phr) {
    strncpy(res->version, ver, MAX_VER_L);
    res->code = code;
    strncpy(res->phrase, phr, MAX_PHRASE_L);

    return 0;
}

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
    c->body = body;
}

int serialize_http_response(struct response* res, char* buf, size_t len) {
    size_t total = 0;
    // Status line
    int bytes = snprintf(buf, len, "%s %d %s\r\n", res->version, res->code, res->phrase);
    if (bytes < 0 || bytes > len) return -1;
    total += bytes;
    // Headers
    for (int i = 0; i < res->content.headers_n; i++) {
        bytes = snprintf(buf+total, len-total, "%s: %s\r\n", res->content.headers[i].name, res->content.headers[i].value);
        if (bytes < 0 || bytes > len-total) return -1;
        total += bytes;
    }
    // \r\n
    memcpy(buf+total, "\r\n", 2);
    total += 2;

    return total;
}

int deserialize_http_request(char* buf, size_t len, struct request* req) {
    
}