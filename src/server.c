#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define BACK_LOG 128
#define BUF_SIZE 4096

int break_flag = 1;

void send_all(int fd, char* buf, ssize_t bytes) {
    ssize_t total = 0;
    ssize_t sent = 0;
    while (total < bytes) {
        sent = send(fd, buf, BUF_SIZE, 0);
        if (sent == -1) {
            perror("Send error: ");
            continue;
        }
        total += sent;
    }
}

void try_close(int fd) {
    printf("Close socket <%d>...", fd);
    if (close(fd) == -1) {
        perror("Close error: ");
        exit(EXIT_FAILURE);
    }
    printf(" done\n");
}

void sigint_handler() {
    break_flag = 0;
}

int main(int argc, char** argv) {
    printf("Started...\n");
    // Args
    if (argc != 2) {
        printf("Usage: %s <port>", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);

    // SIGINT handler
    printf("Setup sigint handler...");
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("SIGINT handler error: ");
        exit(EXIT_FAILURE);
    }
    printf(" done\n");

    // Socket address
    printf("Create socket...");
    struct sockaddr_in s_addr = {0};
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(port);

    // Socket creation
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss == -1) {
        perror("Server socket creation error: ");
        exit(EXIT_FAILURE);
    }
    printf(" done\n");

    // Reuse port
    printf("Reuse port...");
    int opt = 1;
    if (setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Sockopt error: ");
        try_close(ss);
        exit(EXIT_FAILURE);
    }
    printf(" done\n");

    // Bind socket
    printf("Bind socket...");
    if (bind(ss, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1) {
        perror("Bind error: ");
        try_close(ss);
        exit(EXIT_FAILURE);
    }
    printf(" done\n");

    // Listen on socket
    if (listen(ss, BACK_LOG) == -1) {
        perror("Listen error: ");
        try_close(ss);
        exit(EXIT_FAILURE);
    }
    printf("Listen on %d\n", port);

    // Main cycle
    char* buffer_in = malloc(BUF_SIZE);
    char* buffer_out = malloc(BUF_SIZE);
    while (break_flag) {
        // Client accept
        printf("Wait new connection...\n");
        struct sockaddr_in c_addr = {0};
        socklen_t len = sizeof(c_addr);
        int cs = accept(ss, (struct sockaddr*)&c_addr, &len);
        if (cs == -1) {
            perror("Accept error: ");
            continue;
        }
        printf("\n====\n");
        printf("New connection\n");
        printf("Host: %s:%d\nSocket: %d\n", inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port), cs);
        printf("====\n");

        // Read cycle
        while (break_flag) {
            ssize_t bytes = recv(cs, buffer_in, BUF_SIZE, 0);
            if (bytes == -1) {
                perror("Recv error: ");
                continue;
            }
            if (bytes == 0) {
                printf("Connectoin closed by client\n");
                break;
            }

            memcpy(buffer_out, buffer_in, bytes);
            memset(buffer_in, 0, bytes);

            send_all(cs, buffer_out, bytes);
            memset(buffer_out, 0, bytes);
        }

        try_close(cs);
    }
    free(buffer_in);
    free(buffer_out);

    try_close(ss);

    printf("Shut down...\n");
    exit(EXIT_SUCCESS);
}