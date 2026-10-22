#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#include "server.h"


#define MAX_ATTEMPTS 3
#define TIMEOUT 10

void stop_server(int pid) {
    kill(pid, SIGINT);
    waitpid(pid, NULL, 0);
}

void test_server_many_conn(void) {
    int port = 8888;
    int pid = fork();

    // Parent process
    if (pid < 0) { 
        CU_FAIL("Fork failed!\n"); 
        return; 
    }
    else
    // Chiled process
    if (pid == 0) {
        // Output >> null
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        int status = start(port);
        _exit(status);
    }
     // Parent process
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    for (int j = 0; j < 3; j++) {
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if (s < 0) {
            CU_FAIL("Socket error!\n");
            stop_server(pid);
            return;
        }

        // Recv timeout
        struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            CU_FAIL("Connection error!\n");
            close(s);
            stop_server(pid);
        }

        // Connection
        int connected = 0;
        for (int i = 0; i < MAX_ATTEMPTS; i++) {
            if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
                connected = 1;
                break;
            }
            sleep(1);
        }
        // Connection error
        if (connected == 0) {
            CU_FAIL("Connection error!\n");
            close(s);
            stop_server(pid);
            return;
        }

        char* msg = "test";
        size_t size = strlen(msg);

        CU_ASSERT_EQUAL(send_all(s, msg, size), 0);

        char* buffer = malloc(size);
        ssize_t bytes = recv(s, buffer, size, 0);
        CU_ASSERT_TRUE(bytes > 0);
        free(buffer);

        close(s);
    }

    stop_server(pid);
}

void test_server_ping(void) {
    int port = 8888;
    int pid = fork();

    // Parent process
    if (pid < 0) { 
        CU_FAIL("Fork failed!\n"); 
        return; 
    }
    else
    // Chiled process
    if (pid == 0) {
        // Output >> null
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        int status = start(port);
        _exit(status);
    }
    // Parent process
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        CU_FAIL("Socket error!\n");
        stop_server(pid);
        return;
    }

    // Recv timeout
    struct timeval tv = { .tv_sec = 2, .tv_usec = 0 };
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        CU_FAIL("Connection error!\n");
        close(s);
        stop_server(pid);
    }

    // Connection
    int connected = 0;
    for (int i = 0; i < MAX_ATTEMPTS; i++) {
        if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            connected = 1;
            break;
        }
        sleep(1);
    }
    // Connection error
    if (connected == 0) {
        CU_FAIL("Connection error!\n");
        close(s);
        stop_server(pid);
        return;
    }

    char* msg = "test";
    size_t size = strlen(msg);

    CU_ASSERT_EQUAL(send_all(s, msg, size), 0);

    char* buffer = malloc(size);
    ssize_t bytes = recv(s, buffer, size, 0);
    CU_ASSERT_TRUE(bytes > 0);
    free(buffer);

    close(s);
    stop_server(pid);
}