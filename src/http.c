#include <stdio.h>
#include <string.h>

#include "http.h"


int deserialize_request(char* text, size_t len, struct request* req) {
    (void)text;
    (void)len;
    (void)req;
    return 0;
}

int serialize_response(struct response* res, char* text, size_t len) {
    int n = snprintf(text, len, "%s %d %s\r\n", res->version, res->status, res->phrase);
    if (n < 0 || n >= (int)len) return -1;

    for (int i = 0; i < res->content.headers_n; i++) {
        struct header header = res->content.headers[i];
        char header_str[MAX_NAME_L + MAX_VAL_L + 6]; // 6 = ": " + \r\n\r\n

        int k = snprintf(header_str, sizeof(header_str), "%s: %s\r\n", header.name, header.value);
        if (k < 0 || k >= (int)sizeof(header_str)) return -1;
        
        strncat(text, header_str, k);
    }

    strncat(text, "\r\n", 3);
    strncat(text, res->content.body, res->content.body_len);

    return 0;
}