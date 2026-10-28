#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

#include <pthread.h>
#include <stdatomic.h>

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "queue.h"

#define BACKLOG 128
#define IP_ADDR_S 32
#define BUF_SIZE 4096
#define MAX_THREADS 8
#define RECV_TIMEOUT 1
#define ACCEPT_TIMEOUT 1


volatile sig_atomic_t break_flag = 1;
struct queue jobs = {0};
pthread_mutex_t lock;
pthread_cond_t not_empty;
atomic_bool stopping = 0;

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

void try_close(int fd) {
    if (close(fd) < 0)
        perror("Socket close error");
    else
        printf("Socket <%d> closed\n", fd);
}

void* work(void* args) {
    (void)args;
    int cs = -1;
    // Buffer init
    char* buffer = malloc(BUF_SIZE);
    if (buffer == NULL) {
        perror("Memory allocation error");
        return NULL;
    }
    while (!stopping) {
        pthread_mutex_lock(&lock);
        while (jobs.size == 0 && !stopping) 
            pthread_cond_wait(&not_empty, &lock);
        if (!stopping && jobs.size > 0) {
            cs = queue_get(&jobs);
            printf("Thread[%lu] : got job <%d>\n", pthread_self(), cs);
        }
        pthread_mutex_unlock(&lock);
        // Recv cycle
        while (!stopping) {
            ssize_t bytes = recv(cs, buffer, BUF_SIZE, 0);
            // Recv error
            if (bytes < 0) {
                if (errno == EAGAIN)
                    continue;
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
        if (cs != -1)
            try_close(cs);
        cs = -1;
    }
    free(buffer);

    return NULL;
}

void sigint_handler(int sig) {
    (void)sig;
    break_flag = 0;
}

int start(int port) {
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
        try_close(ss);
        return -1;
    }
    // Recv timeout setup
    struct timeval timeout = {0};
    timeout.tv_sec = ACCEPT_TIMEOUT;
    if (setsockopt(ss, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        perror("Accept timeout error");
        try_close(ss);
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
    // Threads
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_t workers[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) 
        pthread_create(&workers[i], NULL, &work, NULL);
    // Main cycle
    printf("Server started...\n");
    int counter = 0;
    while (break_flag) {
        // Client address
        struct sockaddr_in c_addr = {0};
        socklen_t c_sock_len = sizeof(c_addr);
        // Accept new connection
        int cs = accept(ss, (struct sockaddr*)&c_addr, (socklen_t*)&c_sock_len);
        if (cs < 0) {
            if (errno == EAGAIN)
                continue;
            if (errno != EINTR)
                perror("Accept error");
            continue;
        }
        counter++;
        // Recv timeout setup
        struct timeval c_timeout = {0};
        c_timeout.tv_sec = RECV_TIMEOUT;
        if (setsockopt(cs, SOL_SOCKET, SO_RCVTIMEO, &c_timeout, sizeof(c_timeout))) {
            perror("Recv timeout error");
            continue;
        }
        // Log client socket info
        char ip_addr[IP_ADDR_S];
        int c_port = ntohs(c_addr.sin_port);
        inet_ntop(AF_INET, &c_addr.sin_addr, ip_addr, sizeof(ip_addr));
        printf("\n====\nNew connection #%d\nHost: %s:%d\n====\n", counter, ip_addr, c_port);
        // Create job
        pthread_mutex_lock(&lock);
        queue_add(&jobs, cs);
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
    }
    // Stop threads
    pthread_mutex_lock(&lock);
    stopping = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&lock);
    // Wait threads
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(workers[i], NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty);

    try_close(ss);

    printf("Shut down...\n");
    return 0;
}