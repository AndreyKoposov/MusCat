#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#include "server.h"


#define MAX_ATTEMPTS 3

void stop_server(int pid) {
    kill(pid, SIGINT);
    waitpid(pid, NULL, 0);
}

void test_server(void) {
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

    int max_size = 10000;
    char* long_msg = malloc(max_size);
    memset(long_msg, 'a', max_size);
    char* msgs[3] = {"Hi", "Long Long Long String", long_msg};
    for (int i = 0; i < 3; i++) {
        size_t size = strlen(msgs[i]);

        CU_ASSERT_EQUAL(send_all(s, msgs[i], size), 0);

        char* buffer = malloc(size);
        size_t total = 0;
        while (total < size) {
            ssize_t bytes = recv(s, buffer+total, size-total, 0);
            if (bytes <= 0) break;
            total += bytes;
        }

        CU_ASSERT_EQUAL(total, size);
        CU_ASSERT_NSTRING_EQUAL(msgs[i], buffer, size);
        free(buffer);
    }
    free(long_msg);

    close(s);
    stop_server(pid);
}

int main() {
  CU_pSuite pSuite = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  /* add a suite to the registry */
  pSuite = CU_add_suite("Suite", NULL, NULL);
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* add the tests to the suite */
  /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
  CU_pTest success = CU_add_test(pSuite, "test of server", test_server);
  if (success == NULL) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}