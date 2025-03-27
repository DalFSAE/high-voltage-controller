
#include "app_main.h"
#include "hvc_config.h"
#include "hv_control.h"
#include "adc_driver.h"
#include "debug_uart.h"

#include "stm32g0xx_hal.h"
#include "core_cm0plus.h"
#include "main.h"

#include "debug_uart.h"

#include <stdio.h>

extern UART_HandleTypeDef huart5;



void app_init() {
    adc_init();
    debug_uart_init(&huart5);
    debug_print("[DEBUG] HVC STARTED\n");

    return;
}

void app_main() {
    // bool debug = false;


    HVC_State_t state = HVC_STANDBY;

    uint32_t previousTick = 0;
    uint32_t interval = 1000; // Interval in milliseconds    

    while (true) {
        // Precharge
        if (sdc_present() && state == HVC_STANDBY) {
            // simple_precharge();
            state = active_precharge();
        }
        // Check for SDC   
        if (!sdc_present() && state == HVC_TS_ENERGIZED) {
            // debug_print("[DEBUG] HVC STANDBY\n");
            disable_all_relays();
            state = HVC_STANDBY;
        }
        // Status LED
        if (HAL_GetTick() - previousTick > interval ) {
            previousTick = HAL_GetTick();  
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Status LED
            debug_printf("[DEBUG] Tick: %lu\n", HAL_GetTick());
            debug_printf("[DEBUG] HVC State: %d\n", state);
            print_hv_adc_data();

        }        
    }
}


