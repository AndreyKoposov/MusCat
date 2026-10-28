#include <CUnit/Basic.h>

#include "queue.h"


void test_add(void) {
    // Arrange
    struct queue q = {0};

    // Act
    queue_add(&q, 1);
    queue_add(&q, 2);
    queue_add(&q, 3);
    queue_add(&q, 4);

    // Assert
    CU_ASSERT_EQUAL(q.size, 4);
    CU_ASSERT_EQUAL(queue_get(&q), 1);
    CU_ASSERT_EQUAL(queue_get(&q), 2);
    CU_ASSERT_EQUAL(queue_get(&q), 3);
    CU_ASSERT_EQUAL(queue_get(&q), 4);
}

void test_get(void) {
    // Arrange
    struct queue q = {0};
    queue_add(&q, 1);
    queue_add(&q, 2);
    queue_add(&q, 3);
    queue_add(&q, 4);

    // Act
    int val1 = queue_get(&q);
    int val2 = queue_get(&q);
    int val3 = queue_get(&q);
    int val4 = queue_get(&q);

    // Assert
    CU_ASSERT_EQUAL(q.size, 0);
    CU_ASSERT_EQUAL(val1, 1);
    CU_ASSERT_EQUAL(val2, 2);
    CU_ASSERT_EQUAL(val3, 3);
    CU_ASSERT_EQUAL(val4, 4);
}