#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "http.h"


#define BACK_LOG 128
#define BUF_SIZE 4096

volatile sig_atomic_t break_flag = 1;

// Send all bytes from buffer
int send_all(int fd, char* buf, ssize_t bytes) {
    ssize_t total = 0;
    ssize_t sent = 0;
    while (total < bytes) {
        sent = send(fd, buf+total, bytes-total, 0);
        if (sent == -1) {
            perror("Send error");
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (sent == 0) return -1;
        total += sent;
    }

    return 0;
}

// Close fd with error check
void try_close(int fd) {
    printf("Close socket <%d>...", fd);
    if (close(fd) == -1) {
        perror("Close error");
        return;
    }
    printf(" done\n");
}

// Handler to interrupt main cycle
void sigint_handler(int sig) {
    (void)sig;
    break_flag = 0;
}

// Statrs server on port
int start(int port) {
    printf("Started...\n");

    // SIGINT handler
    printf("Setup sigint handler...");
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("SIGINT handler error");
        return -1;
    }
    printf(" done\n");

    // SIGPIPE ignore
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("SIGPIPE ignore error");
        return -1;
    }

    // Socket address
    printf("Create socket...");
    struct sockaddr_in s_addr = {0};
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(port);

    // Socket creation
    int ss = socket(AF_INET, SOCK_STREAM, 0);
    if (ss == -1) {
        perror("Server socket creation error");
        return -1;
    }
    printf(" done\n");

    // Reuse port
    printf("Reuse port...");
    int opt = 1;
    if (setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Sockopt error");
        try_close(ss);
        return -1;
    }
    printf(" done\n");

    // Bind socket
    printf("Bind socket...");
    if (bind(ss, (struct sockaddr*)&s_addr, sizeof(s_addr)) == -1) {
        perror("Bind error");
        try_close(ss);
        return -1;
    }
    printf(" done\n");

    // Listen on socket
    if (listen(ss, BACK_LOG) == -1) {
        perror("Listen error");
        try_close(ss);
        return -1;
    }
    printf("Listen on %d\n", port);

    int con_counter = 0;
    char* buffer = malloc(BUF_SIZE);
    if (buffer == NULL) return -1;

    // Main cycle
    while (break_flag) {
        // Client accept
        printf("Wait new connection...\n");
        struct sockaddr_in c_addr = {0};
        socklen_t len = sizeof(c_addr);
        int cs = accept(ss, (struct sockaddr*)&c_addr, &len);
        if (cs == -1) {
            if (errno == EINTR) continue;
            perror("Accept error");
            continue;
        }
        con_counter++;

        char address[32];
        printf("\n====\n");
        printf("New connection #%d\n", con_counter);
        printf("Host: %s:%d\nSocket: %d\n", 
                inet_ntop(AF_INET, &c_addr.sin_addr, address, sizeof(address)), 
                ntohs(c_addr.sin_port), cs);
        printf("====\n");

        // Read cycle
        char* sep;
        while (break_flag) {
            ssize_t bytes = recv(cs, buffer, BUF_SIZE, 0);
            // Recv error
            if (bytes == -1) {
                if (errno == EINTR) continue;
                perror("Recv error");
                break;
            }
            // Connection closed
            if (bytes == 0) {
                printf("Connectoin closed by client\n");
                break;
            }
            sep = strstr(buffer, "\r\n\r\n");
            if (sep != NULL) {
                break;
            }
        } 
        // Request parse
        struct request req = {0};
        char* head;
        char* body;
        int status = 200;
        if (sep != NULL) {
            *(sep+2) = '\0';

            head = buffer;
            body = sep + 4;

            status = deserialize_request(head, &req);
        }
        else
            status = 400;
        // Response
        struct response res = {0};
        if (body == NULL)
            body = "<h1>Ошибка в теле!</h1>";
        add_status_line_res(&res, "HTTP/1.1", status, get_phrase(status));
        add_header(&res.content, "Content-Type", "text/html; charset=utf-8");
        add_header(&res.content, "Connection", "close");
        add_date(&res.content);
        add_content_len(&res.content, strlen(body));
        add_body(&res.content, body);
    
        serialize_response(&res, buffer, BUF_SIZE);
        // Send
        if (send_all(cs, buffer, strlen(buffer)) == -1)
            break;     

        memset(buffer, 0, BUF_SIZE);
        try_close(cs);
    }
    free(buffer);

    try_close(ss);

    printf("Shut down...\n");
    return 0;
}