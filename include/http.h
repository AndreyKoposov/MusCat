#ifndef HTTP_H
#define HTTP_H

#include <stdlib.h>

#define MAX_NAME_L 32
#define MAX_VAL_L 64
#define MAX_HEADERS_N 32
#define MAX_METHOD_L 16
#define MAX_VER_L 16
#define MAX_PATH_L 2048
#define MAX_PHRASE_L 32


typedef struct header {
    char name[MAX_NAME_L+1];
    char value[MAX_VAL_L+1];
} header;

typedef struct content {
    int headers_n;
    struct header headers[MAX_HEADERS_N];
    size_t body_len;
    char* body;
} content;

typedef struct request {
    char method[MAX_METHOD_L+1];
    char path[MAX_PATH_L+1];
    char version[MAX_VER_L+1];
    struct content content;
} request;

typedef struct response {
    char version[MAX_VER_L+1];
    int code;
    char phrase[MAX_PHRASE_L+1];
    struct content content;
} response;

int add_status_line_response(struct response* res, char* ver, int code, char* phr);
int add_header(struct content* c, char* name, char* value);
int add_date(struct content* c);
int add_length(struct content* c, size_t len);
int add_body(struct content* c, char* body);

int serialize_http_response(struct response* res, char* buf, size_t len);
int deserialize_http_request(char* buf, size_t len, struct request* req);


#endif