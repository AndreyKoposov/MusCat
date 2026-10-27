#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 128
#define IP_ADDR_S 32
#define BUF_SIZE 4096


volatile sig_atomic_t break_flag;

int send_all(int fd, char* buf, size_t data_len) {
    ssize_t total = 0;
    while (total < data_len) {
        ssize_t bytes = send(fd, buf+total, data_len-total, 0);
        if (bytes <= 0)
            return -1;
        total += bytes;
    }
    return 0;
}

void try_close(int fd) {
    if (close(fd) < 0)
        perror("Socket close error");
}

int start(int port) {
    // Socket address
    struct sockaddr_in s_addr = {0};
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(port);
    // Socket creation
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss < 0) {
        perror("Socket error");
        return -1;
    }
    // Socket bind
    if(bind(ss, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
        perror("Bind error");
        try_close(ss);
        return -1;
    }
    // Listen port
    if (listen(ss, BACKLOG) < 0) {
        perror("Listen error");
        try_close(ss);
        return -1;
    }
    // Main cycle
    int counter = 0;
    while (break_flag) {
        struct sockaddr_in c_addr = {0};
        // Accept new connection
        int cs = accept(ss, (struct sockaddr*)&c_addr, sizeof(c_addr));
        if (cs < 0) {
            perror("Accept error");
            continue;
        }
        counter++;
        // Log client socket info
        char ip_addr[IP_ADDR_S];
        int c_port = ntohs(c_addr.sin_port);
        inet_ntop(AF_INET, &c_addr.sin_addr, ip_addr, sizeof(c_addr));
        printf("\n====\nNew connection #%d\nHost: %s:%d\n====\n", counter, ip_addr, c_port);
        // Buffer init
        char* buffer = malloc(BUF_SIZE);
        if (buffer == NULL) {
            perror("Memory allocation error");
            return -1;
        }
        // Recv cycle
        while (break_flag) {
            ssize_t bytes = recv(cs, buffer, BUF_SIZE, 0);
            // Recv error
            if (bytes < 0) {
                perror("Recv error");
                break;
            }
            // Connection closed
            if (bytes == 0) {
                break;
            }
            // Send all data
            if (send_all(cs, buffer, bytes) < 0) {
                perror("Send error");
                break;
            }
        }

        free(buffer);

        try_close(cs);
    }

    try_close(ss);

    return 0;
}