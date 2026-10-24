#include <stdlib.h>
#include <stdio.h>


int parse(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }
    int port = (int)strtol(argv[1], NULL, 10);

    return port;
}

int str_to_int(char* str) {
    return atoi(str);
}

int int_to_str(int n, char* text, int maxlen) {
    int k = snprintf(text, maxlen, "%d", n);
    if (k < 0 || k >= maxlen) 
        return -1;
    return 0;
}