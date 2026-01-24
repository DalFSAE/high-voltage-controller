#include "unity.h"
#include "integrator.h"

#include "debug_print.h"

void create_object(void) {


    TEST_ASSERT_EQUAL(0, 0);
}

void test_integrator_constant_input(void)
{
    Integrator i;
    integrator_init(&i, 0.0f, 0);


    for (uint32_t now_ms = 0; now_ms <= 1000; now_ms+= 100) {
        integrator_step_ms(&i, 2.0f, now_ms); // 2A for 1s total
        DEBUG_PRINTF("q: %f\n", i.x);
    }
    DEBUG_PRINTF("q: %f\n", i.x);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f, i.x);
}

void run_tests_soc(void) {
    printf("STARTING TESTS");
    RUN_TEST(create_object);
    RUN_TEST(test_integrator_constant_input);
}
