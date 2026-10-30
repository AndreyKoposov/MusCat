#include "core.h"


int validate_request(char* buf, size_t len) {
    struct request req = {0};
    
    return deserialize_http_request(buf, len, &req);
}

int get_ok(char* buf, size_t len) {
    struct response res = {0};  

    if (add_status_line_response(&res, "HTTP/1.1", 200, "OK") < 0)
        return -1;
    if (add_header(&res.content, "Connection", "close") < 0)
        return -1;
    if (add_length(&res.content, 0) < 0)
        return -1;

    return serialize_http_response(&res, buf, len);
}

int get_bad_request(char* buf, size_t len) {
    struct response res = {0}; 

    if (add_status_line_response(&res, "HTTP/1.1", 400, "Bad Rquest") < 0)
        return -1;
    if (add_header(&res.content, "Connection", "close") < 0)
        return -1;
    if (add_length(&res.content, 0) < 0)
        return -1;

    return serialize_http_response(&res, buf, len);
}