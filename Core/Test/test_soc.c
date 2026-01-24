#include "unity.h"
#include "soc"

void create_object(void) {


    TEST_ASSERT_EQUAL(0, 0);
}

void run_tests_soc(void) {
    printf("STARTING TESTS");
    RUN_TEST(create_object)
}
