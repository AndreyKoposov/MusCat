#include <CUnit/Basic.h>

#include "utils.h"


void test_str_to_int_pos(void) {
    // Arrange
    char* str1 = "123";
    char* str2 = "-37";
    char* str3 = "1000000";
    long num1 = 0;
    long num2 = 0;
    long num3 = 0;

    // Act
    int status1 = str_to_int(str1, &num1);
    int status2 = str_to_int(str2, &num2);
    int status3 = str_to_int(str3, &num3);

    // Assert
    CU_ASSERT_EQUAL(status1, 0);
    CU_ASSERT_EQUAL(num1, 123);

    CU_ASSERT_EQUAL(status2, 0);
    CU_ASSERT_EQUAL(num2, -37);

    CU_ASSERT_EQUAL(status3, 0);
    CU_ASSERT_EQUAL(num3, 1000000);
}

void test_str_to_int_neg(void) {
    // Arrange
    char* str = "123garbage";
    long num = 0;

    // Act
    int status = str_to_int(str, &num);

    // Assert
    CU_ASSERT_EQUAL(status, -1);
}

void test_int_to_str(void) {
    // Arrange
    char* str[MAX_NUM_LEN];
    long num = 123;

    // Act
    int status = int_to_str(num, str);

    // Assert
    CU_ASSERT_EQUAL(status, 0);
    CU_ASSERT_NSTRING_EQUAL(str, "123", MAX_NUM_LEN);
}