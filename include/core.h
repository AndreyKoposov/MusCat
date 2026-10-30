#ifndef CORE_H
#define CORE_H

#include "http.h"

int validate_request(char* buf, size_t len);
int get_ok(char* buf, size_t len);
int get_bad_request(char* buf, size_t len);

#endif