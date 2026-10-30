#include <CUnit/Basic.h>

#include "http.h"


void test_serialize_http_response_1(void) {
    // Arrange
    const char* expected = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Content-Type: HTML\r\n"
                           "Content-Length: 0\r\n\r\n";
    int size = strlen(expected);
    char* buf = malloc(size);
    struct response res = {0};
    add_status_line_response(&res, "HTTP/1.1", 200, "OK");
    add_header(&res.content, "Connection", "close");
    add_header(&res.content, "Content-Type", "HTML");
    add_length(&res.content, 0);

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

void test_deserialize_http_request_1(void) {
    // Arrange
    char* buffer = "GET / HTTP/1.1\r\n"
                   "Connection: close\r\n"
                   "Content-Type: HTML\r\n\r\n";
    char* method = "GET";
    char* path = "/";
    char* version = "HTTP/1.1";

    struct request req = {0};

    // Act
    int status = deserialize_http_request(buffer, strlen(buffer), &req);

    // Assert
    CU_ASSERT_EQUAL(status, 0);

    CU_ASSERT_STRING_EQUAL(req.method, method);
    CU_ASSERT_STRING_EQUAL(req.path, path);
    CU_ASSERT_STRING_EQUAL(req.version, version);

    CU_ASSERT_STRING_EQUAL(req.content.headers[0].name, "Connection");
    CU_ASSERT_STRING_EQUAL(req.content.headers[0].value, "close");

    CU_ASSERT_STRING_EQUAL(req.content.headers[1].name, "Content-Type");
    CU_ASSERT_STRING_EQUAL(req.content.headers[1].value, "HTML");

    CU_ASSERT_EQUAL(req.content.headers_n, 2);
}

void test_deserialize_http_request_2(void) {
    // Arrange
    char* wrong1 = "GET /  HTTP/1.1\r\n"
                   "Connection: close\r\n"
                   "Content-Type: HTML\r\n\r\n";
    char* wrong2 = "GET / HTTP/1.1\r\n"
                   "Connection:close\r\n"
                   "Content-Type: HTML\r\n\r\n";
    char* wrong3 = "GET / HTTP/1.1\r\n"
                   "Connection:close\r\n"
                   "Content-Type: HTML\r\n\r\n";   
    char* wrong4 = "GET / HTTP/1.1\r\n"
                   "Connection: close\r\n"
                   "Content-Type: HTML\r\\r\n";
    char* wrong5 = "GET/HTTP/1.1\r\n"
                   "Connection: close\r\n"
                   "Content-Type: HTML\r\\r\n";
    char* wrong6 = "GET / HTTP/1.1 sd \r\n"
                   "Connection: close\r\n"
                   "Content-Type: HTML\r\\r\n";
    struct request req = {0};

    // Act
    int status1 = deserialize_http_request(wrong1, strlen(wrong1), &req);
    int status2 = deserialize_http_request(wrong2, strlen(wrong2), &req);
    int status3 = deserialize_http_request(wrong3, strlen(wrong3), &req);
    int status4 = deserialize_http_request(wrong4, strlen(wrong4), &req);
    int status5 = deserialize_http_request(wrong5, strlen(wrong5), &req);
    int status6 = deserialize_http_request(wrong6, strlen(wrong6), &req);

    // Assert
    CU_ASSERT_EQUAL(status1, -1);
    CU_ASSERT_EQUAL(status2, -1);
    CU_ASSERT_EQUAL(status3, -1);
    CU_ASSERT_EQUAL(status4, -1);
    CU_ASSERT_EQUAL(status5, -1);
    CU_ASSERT_EQUAL(status6, -1);
}