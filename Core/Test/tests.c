
#include "tests.h"

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

    test_failed();

    return true;
}


void run_tests() {
    bool debug = false;
    
    uint32_t start = HAL_GetTick();
    uint32_t status_ms = 100;
    
    // Start-up Tests (add breakpoint and change debug variable) 
    if (debug) {
        contactor_test();
    }

    // Infinite Loop Tests (add breakpoint and change debug variable)
    while (true) {
        if (debug) {
            HAL_Delay(500);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
        
        
        HAL_Delay(10);
        
        // Status LED
        if (HAL_GetTick() - start > status_ms) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Status LED
            break;
        }


    }

}