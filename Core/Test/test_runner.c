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
