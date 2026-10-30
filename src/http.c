#include <stdio.h>
#include <string.h>
#include <time.h>

#include "http.h"


int add_status_line_response(struct response* res, char* ver, int code, char* phr) {
    strncpy(res->version, ver, MAX_VER_L);
    res->code = code;
    strncpy(res->phrase, phr, MAX_PHRASE_L);

    return 0;
}

int add_status_line_request(struct request* req, char* method, char* path, char* ver) {
    strncpy(req->method, method, MAX_VER_L);
    strncpy(req->path, path, MAX_PATH_L);
    strncpy(req->version, ver, MAX_VER_L);

    return 0;
}

int add_header(struct content* c, char* name, char* value) {
    int i = c->headers_n;
    if (i >= MAX_HEADERS_N) 
        return -1;

    struct header h = {0};
    strncpy(h.name, name, MAX_NAME_L);
    strncpy(h.value, value, MAX_VAL_L);

    c->headers[i] = h;
    c->headers_n++;

    return 0;
}

int add_date(struct content* c) {
    time_t current;
    time(&current);
    struct tm* current_gm = gmtime(&current);

    char time_str[MAX_VAL_L+1];
    strftime(time_str, MAX_VAL_L, "%a, %d %b %Y %H:%M:%S GMT", current_gm);

    return add_header(c, "Date", time_str);
}

int add_length(struct content* c, size_t len) {
    char value[MAX_VAL_L+1];
    int bytes = snprintf(value, MAX_VAL_L, "%lu", len);
    if (bytes < 0 || bytes > MAX_VAL_L) return -1;

    c->body_len = len;
    
    return add_header(c, "Content-Length", value);
}

int add_body(struct content* c, char* body) {
    c->body = body;

    return 0;
}

int deserialize_header(struct content* c, char* h) {
    char* sep = strstr(h, ": ");
    if (sep == NULL) return -1;
    *sep = '\0';
    char* name = h;
    char* value = sep+2;

    return add_header(c, name, value);
}

int serialize_http_response(struct response* res, char* buf, size_t len) {
    size_t total = 0;
    // Status line
    int bytes = snprintf(buf, len, "%s %d %s\r\n", res->version, res->code, res->phrase);
    if (bytes < 0 || (size_t)bytes > len) return -1;
    total += bytes;
    // Headers
    for (int i = 0; i < res->content.headers_n; i++) {
        bytes = snprintf(buf+total, len-total, "%s: %s\r\n", res->content.headers[i].name, res->content.headers[i].value);
        if (bytes < 0 || (size_t)bytes > len-total) return -1;
        total += bytes;
    }
    // \r\n
    memcpy(buf+total, "\r\n", 2);
    total += 2;

    return total;
}

int deserialize_http_request(char* buf, size_t len, struct request* req) {
    char* str = malloc(len+1);
    memcpy(str, buf, len);
    str[len] = '\0';
    // End of head
    char* eoh = strstr(str, "\r\n\r\n");
    if (eoh == NULL) {
        free(str);
        return -1;
    }
    // Method
    char* sep1 = strchr(str, ' ');
    if (sep1 == NULL || sep1[1] == ' ') {
        free(str);
        return -1;
    }
    *sep1 = '\0';
    char* method = str;
    // Path
    char* sep2 = strchr(sep1+1, ' ');
    if (sep2 == NULL || sep2[1] == ' ') {
        free(str);
        return -1;
    }
    *sep2 = '\0';
    char* path = sep1+1;
    // Version
    char* eol = strstr(sep2+1, "\r\n");
    if (eol == NULL) {
        free(str);
        return -1;
    }
    *eol = '\0';
    char* version = sep2+1;
    // Status line
    if (add_status_line_request(req, method, path, version) < 0) {
        free(str);
        return -1;
    }
    // Headers
    char* ptr = eol+2;
    char* sep = NULL;
    while (ptr < eoh) {
        sep = strstr(ptr, "\r\n");
        if (sep == NULL) {
            free(str);
            return -1;
        }
        *sep = '\0';
        if (deserialize_header(&req->content, ptr) < 0) {
            free(str);
            return -1;
        }
        ptr = sep+2;
    }
    free(str);

    return 0;
}