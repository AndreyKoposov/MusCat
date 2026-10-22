#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "server.h"
#include "args_parse.h"


int main(int argc, char** argv) {
    setlocale(LC_TIME, "C");

    int port = parse(argc, argv);
    if (port < 0)
        return -1;
    else
        return start(port);
}