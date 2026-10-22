#include <CUnit/Basic.h>

#include "test_server.h"
#include "test_http.h"


int main() {
  CU_pSuite serverS = NULL;
  CU_pSuite httpS = NULL;

  /* initialize the CUnit test registry */
  if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

  /* add a suite to the registry */
  serverS = CU_add_suite("Server Suite", NULL, NULL);
  httpS = CU_add_suite("HTTP Suite", NULL, NULL);
  if (NULL == serverS || NULL == httpS) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  CU_pTest success = NULL;
  // success = CU_add_test(pSuite, "Test long message", test_server_long_msg);
  // success = CU_add_test(pSuite, "Test many connections", test_server_many_conn);
  success = CU_add_test(httpS, "Test serialize", test_serialize);
  success = CU_add_test(httpS, "Test add status line", test_add_status_line_res_pos);
  success = CU_add_test(httpS, "Test add status line negative", test_add_status_line_res_neg);
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