
#include "tests.h"

#include "stm32g0xx_hal.h"
#include "core_cm0plus.h"

#define DHCSR (*(volatile uint32_t *)0xE000EDF0)
#define C_DEBUGEN_Msk (1UL << 0)



bool is_debugger_attached(void);

bool is_debugger_attached(void) {
    return (DHCSR & C_DEBUGEN_Msk);
}



bool contactor_test() {

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);

    enable_precharge_relay();
    HAL_Delay(1000);
    disable_precharge_relay();
    
    HAL_Delay(1000);
    enable_air_negative();
    HAL_Delay(1000);
    disable_air_negative();


    HAL_Delay(1000);
    enable_air_positive();
    HAL_Delay(1000);
    disable_air_positive();
    // enable_precharge_relay();
    // HAL_Delay(5000);

    // enable_air_positive();
    // HAL_Delay(100);
    // disable_precharge_relay();
    // HAL_Delay(5000);
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);


    return true;
}


void run_tests() {
    // Start-up Tests 

    // Infinite Loop
    while (true) {
        if (!is_debugger_attached()) {
            HAL_Delay(500);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    }

}