#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 128
#define IP_ADDR_S 32
#define BUF_SIZE 4096
#define MAX_THREADS 8


volatile sig_atomic_t break_flag = 1;
pthread_mutex_t lock;

int send_all(int fd, char* buf, ssize_t data_len) {
    ssize_t total = 0;
    while (total < data_len) {
        ssize_t bytes = send(fd, buf+total, data_len-total, 0);
        if (bytes <= 0)
            return -1;
        total += bytes;
    }
    return 0;
}

void* work(void* args) {
    int cs = *(int*)args;
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
            if (errno != EINTR)
                perror("Recv error");
            break;
        }
        // Connection closed
        if (bytes == 0) {
            printf("Connection closed by client\n");
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

    return NULL;
}

void try_close(int fd) {
    if (close(fd) < 0)
        perror("Socket close error");
    else
        printf("Socket <%d> closed\n", fd);
}

void sigint_handler(int sig) {
    (void)sig;
    break_flag = 0;
}

int start(int port) {
    // Threads
    pthread_mutex_init(&lock, NULL);
    pthread_t workers[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) 
        pthread_create(workers[i], NULL, &work, NULL);
    // Setup SIGINT handler
    struct sigaction sa = {0};
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask); // Block additional signals during handler execution
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("SIGINT handler setup error");
        return -1;
    }
    // Setup SIGPIPE ignore
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("SIGPIPE ignore setup error");
        return -1;
    }
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
    // Socket reuse address
    int opt = 1;
    if (setsockopt(ss, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Reuse address error");
        return -1;
    }
    // Socket bind
    if(bind(ss, (struct sockaddr*)&s_addr, (socklen_t)sizeof(s_addr)) < 0) {
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
    printf("Server started...\n");
    int counter = 0;
    while (break_flag) {
        printf("Waiting for new connections...\n");
        // Client address
        struct sockaddr_in c_addr = {0};
        socklen_t c_sock_len = sizeof(c_addr);
        // Accept new connection
        int cs = accept(ss, (struct sockaddr*)&c_addr, (socklen_t*)&c_sock_len);
        if (cs < 0) {
            if (errno != EINTR)
                perror("Accept error");
            continue;
        }
        counter++;
        // Log client socket info
        char ip_addr[IP_ADDR_S];
        int c_port = ntohs(c_addr.sin_port);
        inet_ntop(AF_INET, &c_addr.sin_addr, ip_addr, sizeof(c_addr));
        printf("\n====\nNew connection #%d\nHost: %s:%d\n====\n", counter, ip_addr, c_port);
    }

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(workers[i], NULL);

    try_close(ss);

    printf("Shut down...");
    return 0;
}