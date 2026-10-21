#include <stdio.h>
#include <string.h>
#include <time.h>

#include "http.h"


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
        return -1;
    else
    if (strncmp("HTTP/1.1", version, MAX_VERS_L) == 0)
        return 0;
    else 
    if (strncmp("HTTP/1.0", version, MAX_VERS_L) == 0)
        return 0;
    else
        return -1;
}

int validate_status(int status) {
    if (status < 100 || status >= 600)
        return -1;
    else
        return 0;
}

int validate_phrase(int status, const char* phrase) {
    if (strlen(phrase) > MAX_PHRASE_L) 
        return -1;
    else
    if (strncmp(get_phrase(status), phrase, MAX_PHRASE_L) != 0)
        return -1;
    else
        return 0;
}

int deserialize_request(char* text, size_t len, struct request* req) {
    (void)text;
    (void)len;
    (void)req;
    return 0;
}

int serialize_response(struct response* res, char* text, size_t maxlen) {
    int n = snprintf(text, maxlen, "%s %d %s\r\n", res->version, res->status, res->phrase);
    if (n < 0 || n >= (int)maxlen) return -1;

    for (int i = 0; i < res->content.headers_n; i++) {
        struct header header = res->content.headers[i];
        char header_str[MAX_NAME_L + MAX_VAL_L + 7]; // 6 = ": " + "\r\n\r\n" + "\0"

        int k = snprintf(header_str, sizeof(header_str), "%s: %s\r\n", header.name, header.value);
        if (k < 0 || k >= (int)sizeof(header_str)) return -1;
        
        strncat(text, header_str, k);
    }

    strcat(text, "\r\n");
    if (res->content.body != NULL)
        strncat(text, res->content.body, res->content.body_len);

    return 0;
}

int add_status_line_res(struct response* res, const char* version, int status, const char* phrase) {
    if (validate_version(version) != 0) return -1;
    if (validate_status(status) != 0) return -1;
    if (validate_phrase(status, phrase) != 0) return -1;

    strcpy(res->version, version);
    res->status = status;
    strcpy(res->phrase, phrase);

    return 0;
}

int add_status_line_req(struct request* req, const char* version, const char* path, const char* method) {
    (void)req;
    (void)version;
    (void)path;
    (void)method;
    return 0;
}

int add_header(struct response* res, const char* name, const char* value) {
    if (strlen(name) > MAX_NAME_L) return -1;
    if (strlen(value) > MAX_VAL_L) return -1;

    struct header header = {0};
    strcpy(header.name, name);
    strcpy(header.value, value);

    int index = res->content.headers_n;
    res->content.headers[index] = header;
    res->content.headers_n++;

    return 0;
}

int add_date(struct response* res) {
    time_t current;
    time(&current);
    const struct tm* current_gm = gmtime(&current);

    char time_str[MAX_VAL_L];
    strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", current_gm);

    add_header(res, "Date", time_str);

    return 0;
}

int add_body(struct response* res, char* body) {
    int body_len = -1;
    if (body == NULL) {
        body_len = 0;
    } else {
        body_len = strlen(body);
    }

    // int -> str
    char len_str[9];
    int n = snprintf(len_str, sizeof(len_str), "%d", body_len);
    if (n < 0 || n >= (int)sizeof(len_str)) return -1;

    add_header(res, "Content-Length", len_str);
    res->content.body_len = body_len;
    res->content.body = body;

    return 0;
}