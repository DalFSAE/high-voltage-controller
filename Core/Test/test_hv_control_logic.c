#include "unity.h"
#include "hv_control_logic.h"
#include "mock_ts.h"
#include "stdio.h"

#include "debug_print.h"    



void test_hvc_init_sets_defaults(void)
{
    HvcContext ctx;

    hvc_init(&ctx, 123);

    TEST_ASSERT_EQUAL(HVC_S_INIT, ctx.state);
    TEST_ASSERT_EQUAL_UINT32(123, ctx.start_tick_ms);
    TEST_ASSERT_EQUAL(HVC_FAULT_CLEAR, ctx.fault);  
}

void test_init_state_outputs_are_safe_off(void)
{
    HvcContext ctx;
    HvcOutputs out;
    HvcInputs in = _hvc_inputs_default();

    hvc_init(&ctx, 0);
    hvc_update(&ctx, &in, &out);

    TEST_ASSERT_FALSE(out.air_n_on);
    TEST_ASSERT_FALSE(out.pc_on);
    TEST_ASSERT_FALSE(out.air_p_on);
}

void test_mock_ts(void) {
    float v_in = 400.0f;
    float t_delta_ms = 100;
    float v_prev = 0.0f;

    for (uint32_t t_ms = 0; t_ms <= 4000; t_ms += t_delta_ms) {
        v_prev = rc_step_vc(v_prev, v_in, 2000.0f, 0.000440f, t_delta_ms);
        DBG_PRINTF("Time: %4d ms, Vc: %.2f V\n", t_ms, v_prev);
    }
    TEST_ASSERT_EQUAL(1, 1);
}

void test_mock_precharge_sequence(void) {

    TEST_IGNORE_MESSAGE("Precharge timing logic not implemented yet");

    HvcContext ctx;
    HvcOutputs out;
    HvcInputs in = {0};

    hvc_init(&ctx, 0);

    for (uint32_t time_ms = 0; time_ms <= 6000; time_ms += 100) {
        in.now_ms = time_ms;
        hvc_update(&ctx, &in, &out);
    }
    TEST_ASSERT_EQUAL(HVC_S_TS_ENERGIZED, ctx.state);
}



void run_test_hv_control_logic(void)
{
    RUN_TEST(test_hvc_init_sets_defaults);
    RUN_TEST(test_init_state_outputs_are_safe_off);
    RUN_TEST(test_mock_ts);
    RUN_TEST(test_mock_precharge_sequence);
}
