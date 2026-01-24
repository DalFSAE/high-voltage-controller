#include "unity.h"
#include "test_hv_control_logic.h"

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(run_test_hv_control_logic);
    return UNITY_END();
}


// int main(void)
// {
//     UNITY_BEGIN();
//     RUN_TEST(test_hvc_init_sets_defaults);
//     RUN_TEST(test_init_state_outputs_are_safe_off);
//     RUN_TEST(test_mock_ts);
//     RUN_TEST(test_mock_precharge_sequence);
//     return UNITY_END();
// }
