#include <stdlib.h>

#include "server/core.h"


int main(int argc, char** argv) {
    char* address = argv[1];
    int port = atoi(argv[2]);

    start(address, port);

    return 0;
}