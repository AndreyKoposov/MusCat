#include <stdlib.h>


#define MAX_HEADERS_N 64
#define MAX_NAME_L 64
#define MAX_VAL_L 256
#define MAX_VERS_L 16
#define MAX_PATH_L 2048
#define MAX_METHOD_L 16
#define MAX_PHRASE_L 32


typedef struct header {
    char name[MAX_NAME_L+1];
    char value[MAX_VAL_L+1];
} header;

typedef struct content {
    int headers_n;
    struct header headers[MAX_HEADERS_N];
    size_t body_len;
    void* body;
} content;

typedef struct request {
    char method[MAX_METHOD_L+1];
    char path[MAX_PATH_L+1];
    char version[MAX_VERS_L+1];
    struct content content;
} request;

typedef struct response {
    char version[MAX_VERS_L+1];
    int status;
    char phrase[MAX_PHRASE_L+1];
    struct content content;
} response;

const char* get_phrase(int status);

int deserialize_request(char* text, struct request* req);

int serialize_response(struct response* res, char* text, size_t len);

int add_status_line_res(struct response* res, const char* version, int status, const char* phrase);

int add_status_line_req(struct request* req, const char* version, const char* path, const char* method);

int add_header(struct content* c, const char* name, const char* value);

int add_date(struct content* c);

int add_body(struct content* c, void* body, size_t body_len);