#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "http.h"
#include "utils.h"


const char* get_phrase(int status) {
    if (status == 200) return "OK"; else
    if (status == 400) return "Bad Request"; else    
    if (status == 404) return "Not Found"; else    
    if (status == 405) return "Method Not Allowed"; else    
    if (status == 500) return "Internal Server Error"; else    
    if (status == 501) return "Not Implemented"; else   
    return NULL;
} 

int validate_version(const char* version) {
    if (strlen(version) > MAX_VERS_L) 
        return 400;
    else
    if (strcmp("HTTP/1.1", version) == 0)
        return 200;
    else 
    if (strcmp("HTTP/1.0", version) == 0)
        return 200;
    else
        return 400;
}

int validate_status(int status) {
    if (status < 100 || status >= 600)
        return 400;
    else
        return 200;
}

int validate_phrase(const char* phrase) {
    if (strlen(phrase) > MAX_PHRASE_L) 
        return 400;
    else
        return 200;
}

int validate_method(const char* method) {
    if (strlen(method) > MAX_METHOD_L) 
        return 400;
    if (strcmp(method, "GET") == 0)
        return 200;
    else
    if (strcmp(method, "POST") == 0)
        return 200;
    else
    if (strcmp(method, "DELETE") == 0)
        return 200;
    else
    if (strcmp(method, "PUT") == 0)
        return 200;
    else
        return 405;
}

int validate_header(const char* name, const char* value) {
    char copy[MAX_NAME_L+1];
    strcpy(copy, name);
    strlwr(copy);

    if (strlen(copy) > MAX_NAME_L || strlen(value) > MAX_VAL_L) 
        return 400;
    if (strcmp(copy, "connection") == 0) return 200; else
    if (strcmp(copy, "date") == 0) return 200; else
    if (strcmp(copy, "content-length") == 0) return 200; else
    if (strcmp(copy, "content-type") == 0) return 200; else
    if (strcmp(copy, "host") == 0) return 200; else
    if (strcmp(copy, "user-agent") == 0) return 200; else
    if (strcmp(copy, "accept") == 0) return 200; else
        return 400;
}

int validate_path(const char* path) {
    if (strlen(path) > MAX_PATH_L) 
        return 400;
    else
        return 200;
}

int serialize_response(struct response* res, char* text, size_t maxlen) {
    // Status line
    int total = snprintf(text, maxlen, "%s %d %s\r\n", res->version, res->status, res->phrase);
    if (total < 0 || total >= (int)maxlen) return -1;

    // Headers
    for (int i = 0; i < res->content.headers_n; i++) {
        struct header header = res->content.headers[i];
        int k = snprintf(text+total, maxlen-total, "%s: %s\r\n", header.name, header.value);
        if (k < 0 || k >= (int)maxlen-total) return -1;
        total += k;
    }

    // \r\n
    int k = snprintf(text+total, maxlen-total, "\r\n");
    if (k < 0 || k >= (int)maxlen-total) return -1;
    total += k;

    // Body
    if (res->content.body != NULL) {
        if (res->content.body_len > maxlen-total) return -1;
        memcpy(text+total, res->content.body, res->content.body_len);
        total += res->content.body_len;
    }
    text[total] = '\0';

    return total;
}

int add_status_line_res(struct response* res, const char* version, int status, const char* phrase) {
    int code = 0;
    if ((code = validate_version(version)) != 200) return code;
    if ((code = validate_status(status)) != 200) return code;
    if ((code = validate_phrase(phrase)) != 200) return code;

    strcpy(res->version, version);
    res->status = status;
    strcpy(res->phrase, phrase);

    return 200;
}

int add_status_line_req(struct request* req, const char* method, const char* path, const char* version) {
    int code = 0;
    if ((code = validate_method(method)) != 200) return code;
    if ((code = validate_path(path)) != 200) return code;
    if ((code = validate_version(version)) != 200) return code;

    strcpy(req->method, method);
    strcpy(req->path, path);
    strcpy(req->version, version);

    return 200;
}

int add_header(struct content* c, const char* name, const char* value) {
    int code = 0;
    if ((code = validate_header(name, value)) != 200) return code;

    struct header header = {0};
    strcpy(header.name, name);
    strcpy(header.value, value);

    int index = c->headers_n;
    if (index >= MAX_HEADERS_N) return 400;
    c->headers[index] = header;
    c->headers_n++;

    return 200;
}

int add_date(struct content* c) {
    time_t current;
    time(&current);
    struct tm current_gm = {0};
    gmtime_r(&current, &current_gm);

    char time_str[MAX_VAL_L];
    strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", &current_gm);

    return add_header(c, "Date", time_str);
}

int add_content_len(struct content* c, size_t len) {
    char len_str[MAX_NUM_LEN];
    int_to_str(len, len_str);
    c->body_len = len;

    return add_header(c, "Content-Length", len_str);
}

int add_body(struct content* c, void* body) {
    if (c->body_len == 0) return 400;

    c->body = body;

    return 200;
}

int deserialize_header(char* h, struct request* req) {
    char* sep = strstr(h, ": ");
    if (sep == NULL) return 400;
    *sep = '\0';
    char* pointer = sep + 2;

    char* name = h;
    char* value = pointer;

    if (strcmp(name, "Content-Length") == 0) {
        long num = 0;
        if (str_to_int(value, &num) == -1) return 400;
        return add_content_len(&req->content, num);
    }

    return add_header(&req->content, name, value);
}

int deserialize_request(char* text, struct request* req) {
    // Status line
    char* end = strstr(text, "\r\n\r\n") + 4;
    int status = 200;
    char* sep = strstr(text, "\r\n");
    if (sep == NULL) return 400;
    *sep = '\0';
    char* pointer = sep + 2;

    const char* method = strtok(text, " ");
    const char* path = strtok(NULL, " ");
    const char* version = strtok(NULL, " ");
    if ((status = add_status_line_req(req, method, path, version)) != 200)
        return status;

    // Headers
    while (pointer+2 < end) {
        sep = strstr(pointer, "\r\n");
        *sep = '\0';
        if ((status = deserialize_header(pointer, req)) != 200)
            return status;
        pointer = sep + 2;
    }

    return 200;
}