#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BACK_LOG 128

int start(int port) {
    // Socket address
    struct sockaddr_in s_addr = {0};
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    s_addr.sin_port = htonl(port);

    // Socket creation
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss == -1) {
        perror("Server socket creation error: ");
        return -1;
    }

    // Bind socket
    if (bind(ss, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1) {
        perror("Bind error: ");
        return -1;
    }

    // Listen on socket
    if (listen(ss, BACK_LOG) == -1) {
        perror("Bind error: ");
        return -1;
    }

    while (1) {
        struct sockaddr* c_addr;
        int cs = accept(ss, c_addr, sizeof(c_addr));
        if (ss == -1) {
            perror("Client socket creation error: ");
            return -1;
        }
    }
}