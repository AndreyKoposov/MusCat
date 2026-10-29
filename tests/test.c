#include <CUnit/Basic.h>

#include "queue_test.h"
#include "http_test.h"


int main() {
    CU_pSuite queue = NULL;
    CU_pSuite http = NULL;

    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    queue = CU_add_suite("Queue", NULL, NULL);
    http = CU_add_suite("HTTP", NULL, NULL);
    if (queue == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_pTest success = NULL;
    success = CU_add_test(queue, "Add", test_add);
    success = CU_add_test(queue, "Get", test_get);

    success = CU_add_test(http, "Serialize 1", test_serialize_http_response_1);
    success = CU_add_test(http, "Serialize 2", test_serialize_http_response_2);
    success = CU_add_test(http, "Serialize 3", test_serialize_http_response_3);
    success = CU_add_test(http, "Deserialize 1", test_deserialize_http_request_1);
    success = CU_add_test(http, "Deserialize 2", test_deserialize_http_request_2);
    if (success == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}