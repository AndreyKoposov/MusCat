#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


#include "utils.h"

int parse(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }
    int port = (int)strtol(argv[1], NULL, 10);

    return port;
}

int str_to_int(char* str, long* num) {
    char* endptr;
    *num = strtol(str, &endptr, 10);
    if (strlen(endptr) > 0)
        return -1;
    return 0;
}

int int_to_str(long n, char* text) {
    int k = snprintf(text, MAX_NUM_LEN, "%ld", n);
    if (k < 0 || k >= MAX_NUM_LEN) 
        return -1;
    return 0;
}

void strlwr(char* str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}