#define _POSIX_C_SOURCE 200809L

#include <CUnit/Basic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#include "server.h"


void test_server(void) {
    int port = 8888;

    int pid = fork();

    if (pid < 0) { 
        CU_FAIL("Fork failed!\n"); 
        return; 
    }
    else
    if (pid == 0) {
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) {
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        start(port);
        _exit(0);
    }
    sleep(1);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(port);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        CU_FAIL("Socket error!\n");
        return;
    }
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        CU_FAIL("Connection error!\n");
        try_close(s);
        return;
    }

    char* msg = "Hi";
    size_t size = strlen(msg);

    send_all(s, msg, size);

    char buffer[32];
    size_t total = 0;
    while (total < size) {
        ssize_t bytes = recv(s, buffer+total, size-total, 0);
        if (bytes <= 0) break;
        total += bytes;
    }

    CU_ASSERT_EQUAL(total, size);
    CU_ASSERT_NSTRING_EQUAL(msg, buffer, size);

    try_close(s);

    kill(pid, SIGINT);
    waitpid(pid, NULL, 0);
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