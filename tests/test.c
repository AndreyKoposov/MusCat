#include <CUnit/Basic.h>

#include "queue_test.h"


int main() {
    CU_pSuite queue = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    queue = CU_add_suite("Queue", NULL, NULL);
    if (queue == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pTest success = NULL;
    success = CU_add_test(queue, "Add", test_add);
    success = CU_add_test(queue, "Get", test_get);
    if (success == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}