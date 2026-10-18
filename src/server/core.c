#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BACK_LOG 128
#define BUF_SIZE 4096

int send_all(int fd, char* buf, ssize_t bytes) {
    ssize_t total = 0;
    ssize_t sent = 0;
    while (total < bytes) {
        sent = send(fd, buf, BUF_SIZE, 0);
        if (sent == -1)
            continue;
        total += sent;
    }

    return 0;
}

int start(char* address, int port) {
    // Socket address
    struct sockaddr_in s_addr = {0};
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(address);
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

    char* buffer_in = malloc(BUF_SIZE);
    char* buffer_out = malloc(BUF_SIZE);
    while (1) {
        // Client accept
        struct sockaddr_in* c_addr;
        int cs = accept(ss, (struct sockaddr*)c_addr, sizeof(c_addr));
        if (ss == -1) {
            perror("Client socket creation error: ");
            return -1;
        }

        // Read cycle
        while (1) {
            ssize_t bytes = recv(cs, buffer_in, BUF_SIZE, 0);
            if (bytes == -1) {
                perror("Recv error: ");
                break;
            }
            if (bytes == 0) {
                break;
            }

            memcpy(buffer_out, buffer_in, bytes);
            memset(buffer_in, 0, BUF_SIZE);

            send_all(cs, buffer_out, bytes);
        }

        close(cs);
    }
    free(buffer_in);
    free(buffer_out);
}