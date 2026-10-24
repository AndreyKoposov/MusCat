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

int process_request(struct request* req, struct response* res) {

}