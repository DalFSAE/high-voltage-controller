#include "unity.h"

#include "test_hv_control_logic.h"
#include "test_soc.h"

void setUp(void) {}
void tearDown(void) {}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(run_test_hv_control_logic);
    RUN_TEST(run_tests_soc);
    return UNITY_END();
}
