#include <stdio.h>
#include <unistd.h>

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG 128


volatile sig_atomic_t break_flag;

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
    while (break_flag) {

    }
}