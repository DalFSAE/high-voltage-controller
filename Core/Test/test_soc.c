#include "unity.h"
#include "debug_print.h"
#include "integrator.h"
#include "soc.h"  
#include "mock_ts.h"


void test_create_soc() {

    TEST_IGNORE_MESSAGE("Not implemented yet");

    float soc = 0.0f;

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0, soc);
}

void test_create_object(void) {
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

void test_differentiator(void) {
    Differentiator d;
    differentiator_init(&d, 0.0f, 0);
    TEST_ASSERT_EQUAL(true, d.initialized);

    // Const value

    // 
}

void test_mock_ts_test_differentiator(void) {

    int errors = 0;

    const float v_in = 400.0f;
    const float R = 2000.0f;
    const float C = 0.000440f;
    const uint32_t dt_ms = 10;

    float v_prev = 0.0f;

    Differentiator d = {0};

    float dvdt0 = differentiator_step_ms(&d, v_prev, 0);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, dvdt0);

    float last_dvdt = INFINITY;

    DEBUG_PRINTF("t_ms,Vc_V,dvdt_Vps,expected_Vps,error_Vps\n");
    for (uint32_t t_ms = dt_ms; t_ms <= 4000; t_ms += dt_ms) {
        float v_now = rc_step_vc(v_prev, v_in, R, C, dt_ms);
        float dvdt = differentiator_step_ms(&d, v_now, t_ms);
        float dvdt_expected = (v_in - v_prev    ) / (R * C);

        DEBUG_PRINTF("%lu,%.6f,%.6f,%.6f,%.6f\n",
                    (unsigned long)t_ms,
                    v_now,
                    dvdt,
                    dvdt_expected,
                    dvdt - dvdt_expected);
        
        if (dvdt < -1e-3f) {
            // DEBUG_PRINTF("FAIL: negative slope\n");
            errors += 1;
        }

        if (dvdt > last_dvdt + 1e-3f) {
            // DEBUG_PRINTF("FAIL: slope not decreasing\n");
            errors += 1;
        }

        if (fabsf(dvdt - dvdt_expected) > 1e-2f) {
            // DEBUG_PRINTF("FAIL: slope mismatch\n");
            errors += 1;
        }

        last_dvdt = dvdt;
        v_prev = v_now;
    }
    TEST_ASSERT_EQUAL(0, errors);
}

void run_tests_soc(void) {
    printf("STARTING TESTS");
    RUN_TEST(test_create_object);
    RUN_TEST(test_integrator_constant_input);
    RUN_TEST(test_create_soc);
    RUN_TEST(test_differentiator);
    RUN_TEST(test_mock_ts_test_differentiator);
}
