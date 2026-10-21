#include <CUnit/Basic.h>
#include <stdlib.h>

#include "http.h"


#define BUF_SIZE 4096

void test_serialize(void) {
    // Arrange
    struct response res = {0};
    char* buffer = malloc(BUF_SIZE);
    int valid = 0;

    char* body = "<h1>Привет Мир!</h1>";
    add_status_line_res(&res, "HTTP/1.1", 200, get_phrase(200));
    add_header(&res, "Content-Type", "text/html; charset=utf-8");
    add_header(&res, "Connection", "close");
    add_body(&res, body);

    const char* expected = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=utf-8\r\n"
                           "Connection: close\r\n"
                           "Content-Length: 29\r\n"
                           "\r\n"
                           "<h1>Привет Мир!</h1>";

    // Act
    int status = serialize_response(&res, buffer, BUF_SIZE);

    // Assert
    CU_ASSERT_EQUAL(status, 0);
    CU_ASSERT_NSTRING_EQUAL(buffer, expected, BUF_SIZE);

    // Clean
    free(buffer);
}
