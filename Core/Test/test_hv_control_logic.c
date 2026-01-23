#include "unity.h"
#include "hv_control_logic.h"

void setUp(void) {}
void tearDown(void) {}

/*
 * Sanity test:
 *  - can call hvc_init
 *  - context fields are set
 */
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
    HvcInputs in = {0};

    hvc_init(&ctx, 0);
    hvc_update(&ctx, &in, &out);

    TEST_ASSERT_FALSE(out.air_n_on);
    TEST_ASSERT_FALSE(out.pc_on);
    TEST_ASSERT_FALSE(out.air_p_on);
}

void test_mock_precharge_sequence(void) {
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



int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hvc_init_sets_defaults);
    RUN_TEST(test_init_state_outputs_are_safe_off);
    RUN_TEST(test_mock_precharge_sequence);
    return UNITY_END();
}
