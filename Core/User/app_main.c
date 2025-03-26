
#include "app_main.h"
#include "hvc_config.h"
#include "hv_control.h"
#include "adc_driver.h"

#include "stm32g0xx_hal.h"
#include "core_cm0plus.h"





void app_init() {
    adc_init();
    return;
}

void app_main() {
    bool debug = false;

    HVC_State_t state = HVC_STANDBY;

    uint32_t previousTick = 0;
    uint32_t interval = 500; // Interval in milliseconds




    // Start-up Tests (add breakpoint and change debug variable) 
    if (debug) {
        // contactor_test();
    }
    
    // Infinite Loop Tests (add breakpoint and change debug variable)
    while (true) {
        if (debug) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        }
        
        // Precharge
        if (sdc_present() && state == HVC_STANDBY) {
            simple_precharge();
            // state = active_precharge();
        } 
        if (!sdc_present()) {
            disable_all_relays();
            state = HVC_STANDBY;
        }

        // Status LED
        if (HAL_GetTick() - previousTick > interval ) {
            previousTick = HAL_GetTick();  
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Status LED
        }
    }
}


