#include <CUnit/Basic.h>

#include "http.h"


void test_serialize_http_response_1(void) {
    // Arrange
    const char* expected = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Content-Type: HTML\r\n\r\n";
    int size = strlen(expected);
    char* buf = malloc(size);
    struct response res = {0};
    add_status_line_response(&res, "HTTP/1.1", 200, "OK");
    add_header(&res.content, "Connection", "close");
    add_header(&res.content, "Content-Type", "HTML");

    // Act
    int status = serialize_http_response(&res, buf, size);

    // Assert
    CU_ASSERT_EQUAL(status, size);
    CU_ASSERT_NSTRING_EQUAL(buf, expected, size);

    // Clean up
    free(buf);
}

void test_serialize_http_response_2(void) {
    // Arrange
    const char* expected = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Content-Type: HTML\r\n\r\n";
    int size = strlen(expected) - 3;
    char* buf = malloc(size);
    struct response res = {0};
    add_status_line_response(&res, "HTTP/1.1", 200, "OK");
    add_header(&res.content, "Connection", "close");
    add_header(&res.content, "Content-Type", "HTML");

    // Act
    int status = serialize_http_response(&res, buf, size);

    // Assert
    CU_ASSERT_EQUAL(status, -1);

    // Clean up
    free(buf);
}

void test_serialize_http_response_3(void) {
    // Arrange
    const char* expected = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Content-Type: HTML\r\n\r\n";
    int size = strlen(expected) + 3;
    char* buf = malloc(size);
    struct response res = {0};
    add_status_line_response(&res, "HTTP/1.1", 200, "OK");
    add_header(&res.content, "Connection", "close");
    add_header(&res.content, "Content-Type", "HTML");

    // Act
    int status = serialize_http_response(&res, buf, size);

    // Assert
    CU_ASSERT_EQUAL(status, strlen(expected));
    CU_ASSERT_NSTRING_EQUAL(buf, expected, strlen(expected));

    // Clean up
    free(buf);
}

void test_deserialize_http_request(void) {

}