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