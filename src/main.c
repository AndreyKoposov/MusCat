#include <stdlib.h>
#include <stdio.h>

#include "server.h"


int main(int argc, char** argv) {
    // Args
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }
    long port = strtol(argv[1], NULL, 10);

    return start(port);
}