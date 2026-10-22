#include <CUnit/Basic.h>
#include <stdlib.h>

#include "http.h"


#define BUF_SIZE 4096

void test_serialize(void) {
    // Arrange
    struct response res = {0};
    char* buffer = malloc(BUF_SIZE);

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
    CU_ASSERT_EQUAL(status, 200);
    CU_ASSERT_NSTRING_EQUAL(buffer, expected, BUF_SIZE);

    // Clean
    free(buffer);
}

void test_add_status_line_res_pos(void) {
    // Arrange
    struct response res1 = {0};
    struct response res2 = {0};
    struct response res3 = {0};
    char* long_phrase = malloc(MAX_PHRASE_L+1);
    memset(long_phrase, 'a', MAX_PHRASE_L);
    long_phrase[MAX_PHRASE_L] = '\0';

    // Act
    int status1 = add_status_line_res(&res1, "HTTP/1.1", 200, get_phrase(200));
    int status2 = add_status_line_res(&res2, "HTTP/1.0", 404, get_phrase(404));
    int status3 = add_status_line_res(&res3, "HTTP/1.1", 200, long_phrase);

    // Assert
    CU_ASSERT_EQUAL(status1, 200);
    CU_ASSERT_EQUAL(status2, 200);
    CU_ASSERT_EQUAL(status3, 200);

    CU_ASSERT_NSTRING_EQUAL(res1.version, "HTTP/1.1", MAX_VERS_L);
    CU_ASSERT_EQUAL(res1.status, 200);
    CU_ASSERT_NSTRING_EQUAL(res1.phrase, "OK", MAX_PHRASE_L);

    CU_ASSERT_NSTRING_EQUAL(res2.version, "HTTP/1.0", MAX_VERS_L);
    CU_ASSERT_EQUAL(res2.status, 404);
    CU_ASSERT_NSTRING_EQUAL(res2.phrase, "Not Found", MAX_PHRASE_L);

    // Clean
    free(long_phrase);
}

void test_add_status_line_res_neg(void) {
    // Arrange
    struct response res = {0};

    char* long_phrase = malloc(MAX_PHRASE_L+2);
    memset(long_phrase, 'a', MAX_PHRASE_L+1);
    long_phrase[MAX_PHRASE_L+1] = '\0';

    char* long_version = malloc(MAX_VERS_L+2);
    memset(long_version, 'a', MAX_VERS_L+1);
    long_version[MAX_VERS_L+1] = '\0';

    // Act
    int status1 = add_status_line_res(&res, "HTTP/1", 200, get_phrase(200));
    int status2 = add_status_line_res(&res, "HTTP/1.0", 67, get_phrase(404));
    int status3 = add_status_line_res(&res, "HTTP/1.1", 500, long_phrase);
    int status4 = add_status_line_res(&res, long_version, 200, get_phrase(200));

    // Assert
    CU_ASSERT_EQUAL(status1, 400);
    CU_ASSERT_EQUAL(status2, 400);
    CU_ASSERT_EQUAL(status3, 400);
    CU_ASSERT_EQUAL(status4, 400);

    // Clean
    free(long_phrase);
}
