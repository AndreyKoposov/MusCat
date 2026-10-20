#include <stdlib.h>
#include <stdio.h>

#include "server.h"
#include "args_parse.h"


int main(int argc, char** argv) {
    int port = parse(argc, argv);
    if (port < 0)
        return -1;
    else
        return start(port);
}