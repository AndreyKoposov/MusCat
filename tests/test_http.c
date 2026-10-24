#include <CUnit/Basic.h>
#include <stdlib.h>

#include "http.h"


#define BUF_SIZE 4096

void test_deserialize(void) {
    // Arrange
    struct request req = {0};
    char text[BUF_SIZE] = "POST / HTTP/1.1\r\nHost: 127.0.0.1:8888\r\n"
                          "User-Agent: curl/8.5.0\r\n"
                          "Accept: */*\r\n"
                          "Content-Length: 0\r\n"
                          "Content-Type: application/x-www-form-urlencoded\r\n"
                          "\r\n";

    // Act
    int status = deserialize_request(text, &req);

    // Assert
    CU_ASSERT_EQUAL(status, 200);

    CU_ASSERT_NSTRING_EQUAL(req.method, "POST", MAX_METHOD_L);
    CU_ASSERT_NSTRING_EQUAL(req.path, "/", MAX_PATH_L);
    CU_ASSERT_NSTRING_EQUAL(req.version, "HTTP/1.1", MAX_VERS_L);

    CU_ASSERT_NSTRING_EQUAL(req.content.headers[0].name, "Host", MAX_NAME_L);
    CU_ASSERT_NSTRING_EQUAL(req.content.headers[0].value, "127.0.0.1:8888", MAX_VAL_L);

    CU_ASSERT_NSTRING_EQUAL(req.content.headers[1].name, "User-Agent", MAX_NAME_L);
    CU_ASSERT_NSTRING_EQUAL(req.content.headers[1].value, "curl/8.5.0", MAX_VAL_L);

    CU_ASSERT_NSTRING_EQUAL(req.content.headers[2].name, "Accept", MAX_NAME_L);
    CU_ASSERT_NSTRING_EQUAL(req.content.headers[2].value, "*/*", MAX_VAL_L);

    CU_ASSERT_NSTRING_EQUAL(req.content.headers[3].name, "Content-Length", MAX_NAME_L);
    CU_ASSERT_NSTRING_EQUAL(req.content.headers[3].value, "0", MAX_VAL_L);

    CU_ASSERT_NSTRING_EQUAL(req.content.headers[4].name, "Content-Type", MAX_NAME_L);
    CU_ASSERT_NSTRING_EQUAL(req.content.headers[4].value, "application/x-www-form-urlencoded", MAX_VAL_L);
}

void test_serialize(void) {
    // Arrange
    struct response res = {0};
    char* buffer = malloc(BUF_SIZE);

    char* body = "<h1>Привет Мир!</h1>";
    add_status_line_res(&res, "HTTP/1.1", 200, get_phrase(200));
    add_header(&res.content, "Content-Type", "text/html; charset=utf-8");
    add_header(&res.content, "Connection", "close");
    add_content_len(&res.content, strlen(body));
    add_body(&res.content, body);

    const char* expected = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html; charset=utf-8\r\n"
                           "Connection: close\r\n"
                           "Content-Length: 29\r\n"
                           "\r\n"
                           "<h1>Привет Мир!</h1>";

    // Act
    int status = serialize_response(&res, buffer, BUF_SIZE);

    // Assert
    CU_ASSERT_EQUAL(status, strlen(expected));
    CU_ASSERT_NSTRING_EQUAL(buffer, expected, BUF_SIZE);

    // Clean
    free(buffer);
}

void test_serialize_overflow(void) {
    // Arrange
    struct response res = {0};
    char* buffer = malloc(BUF_SIZE);

    char* body = malloc(BUF_SIZE*2);
    memset(body, 'A', BUF_SIZE*2);
    add_status_line_res(&res, "HTTP/1.1", 200, get_phrase(200));
    add_header(&res.content, "Content-Type", "text/html; charset=utf-8");
    add_header(&res.content, "Connection", "close");
    add_content_len(&res.content, BUF_SIZE*2);
    add_body(&res.content, body);

    // Act
    int status = serialize_response(&res, buffer, BUF_SIZE);

    // Assert
    CU_ASSERT_EQUAL(status, -1);

    // Clean
    free(body);
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
    int status5 = add_status_line_res(&res, "HTTP/1.1garbage", 200, get_phrase(200));

    // Assert
    CU_ASSERT_EQUAL(status1, 400);
    CU_ASSERT_EQUAL(status2, 400);
    CU_ASSERT_EQUAL(status3, 400);
    CU_ASSERT_EQUAL(status4, 400);
    CU_ASSERT_EQUAL(status5, 400);

    // Clean
    free(long_phrase);
    free(long_version);
}

void test_add_status_line_req_pos(void) {
    // Arrange
    struct request req1 = {0};
    struct request req2 = {0};
    struct request req3 = {0};
    char* long_path = malloc(MAX_PATH_L+1);
    memset(long_path, 'a', MAX_PATH_L);
    long_path[MAX_PATH_L] = '\0';

    // Act
    int status1 = add_status_line_req(&req1, "GET", "/", "HTTP/1.1");
    int status2 = add_status_line_req(&req2, "POST", "/path", "HTTP/1.0");
    int status3 = add_status_line_req(&req3, "DELETE", long_path, "HTTP/1.1");

    // Assert
    CU_ASSERT_EQUAL(status1, 200);
    CU_ASSERT_EQUAL(status2, 200);
    CU_ASSERT_EQUAL(status3, 200);

    CU_ASSERT_NSTRING_EQUAL(req1.version, "HTTP/1.1", MAX_VERS_L);
    CU_ASSERT_NSTRING_EQUAL(req1.path, "/", MAX_PATH_L);
    CU_ASSERT_NSTRING_EQUAL(req1.method, "GET", MAX_METHOD_L);

    CU_ASSERT_NSTRING_EQUAL(req2.version, "HTTP/1.0", MAX_VERS_L);
    CU_ASSERT_NSTRING_EQUAL(req2.path, "/path", MAX_PATH_L);
    CU_ASSERT_NSTRING_EQUAL(req2.method, "POST", MAX_METHOD_L);

    CU_ASSERT_NSTRING_EQUAL(req3.version, "HTTP/1.1", MAX_VERS_L);
    CU_ASSERT_NSTRING_EQUAL(req3.path, long_path, MAX_PATH_L);
    CU_ASSERT_NSTRING_EQUAL(req3.method, "DELETE", MAX_METHOD_L);

    // Clean
    free(long_path);
}

void test_add_status_line_req_neg(void) {
    // Arrange
    struct request req = {0};

    char* long_path = malloc(MAX_PATH_L+2);
    memset(long_path, 'a', MAX_PATH_L+1);
    long_path[MAX_PATH_L+1] = '\0';

    char* long_version = malloc(MAX_VERS_L+2);
    memset(long_version, 'a', MAX_VERS_L+1);
    long_version[MAX_VERS_L+1] = '\0';

    // Act
    int status1 = add_status_line_req(&req, "GET", "/", "HTTP/1");
    int status2 = add_status_line_req(&req, "POST", "/", long_version);
    int status3 = add_status_line_req(&req, "PUT", long_path, "HTTP/1.0");
    int status4 = add_status_line_req(&req, "garbage", "/", "HTTP/1.1");

    // Assert
    CU_ASSERT_EQUAL(status1, 400);
    CU_ASSERT_EQUAL(status2, 400);
    CU_ASSERT_EQUAL(status3, 400);
    CU_ASSERT_EQUAL(status4, 405);

    // Clean
    free(long_path);
    free(long_version);
}


