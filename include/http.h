#define MAX_HEADERS_N 64
#define MAX_NAME_L 64
#define MAX_VAL_L 256

typedef struct header {
    char name[MAX_NAME_L];
    char value[MAX_VAL_L];
} header;

typedef struct content {
    int headers_n;
    struct header headers[MAX_HEADERS_N];
    int body_len;
    char* body;
} content;

typedef struct request {
    char version[16];
    char path[2048];
    char method[16];
    struct content content;
} request;

typedef struct response {
    char version[16];
    int status;
    char phrase[32];
    struct content content;
} response;

int deserialize_request(char* text, size_t len, struct request* req);

int serialize_response(struct response* res, char* text, size_t len);