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

int add_status_line(struct response* res, const char* version, int status, const char* phrase) {
    if (strlen(version) >= MAX_VERS_L) return -1;
    if (strlen(phrase) >= MAX_PHRASE_L) return -1;
    if (status < 100 || status >= 600) return -1;

    strcpy(res->version, version);
    res->status = status;
    strcpy(res->phrase, phrase);

    return 0;
}

int add_header(struct response* res, const char* name, const char* value) {
    if (strlen(name) >= MAX_NAME_L) return -1;
    if (strlen(value) >= MAX_VAL_L) return -1;

    struct header header = {0};
    strcpy(header.name, name);
    strcpy(header.value, value);

    int index = res->content.headers_n;
    res->content.headers[index] = header;
    res->content.headers_n++;

    return 0;
}

int add_date(struct response* res) {
    add_header(res, "Date", "Time");

    return 0;
}

int add_body(struct response* res, char* body) {
    int body_len = -1;
    if (body == NULL) {
        body_len = 0;
    } else {
        body_len = strlen(body);
    }

    char len_str[8];
    snprintf(len_str, sizeof(len_str), "%d", body_len);
    add_header(res, "Content-Length", len_str);
    res->content.body_len = body_len;
    res->content.body = body;

    return 0;
}