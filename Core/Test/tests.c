
#include "tests.h"
#include "hvc_config.h"
#include "hv_control.h"

#include "stm32g0xx_hal.h"
#include "core_cm0plus.h"



void test_failed() {
    while (true) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Status LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_11); // Status LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12); // Status LED
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // Status LED  
        HAL_Delay(250);
    }

}

    bool test_iso_voltage() {
        return true;
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


    return true;
}


void run_tests() {
    return;
}

