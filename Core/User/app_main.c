
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
    
    debug_print("#######################################\n");
    debug_print("# HIGH VOLTAGE CONTROLLER INITIALIZED #\n");
    debug_print("#######################################\n");
    
    enable_shutdown_circuit();
    return;
}

void app_main() {
    // bool debug = false;


    HVC_State_t state = HVC_STANDBY;

    uint32_t previousTick = 0;
    uint32_t interval = 1000; // Interval in milliseconds    

    uint32_t debug = 0;

    if (debug == 1) {
        enable_air_negative();
        HAL_Delay(1000);
        disable_air_negative();
        HAL_Delay(1000);

        enable_air_positive();
        HAL_Delay(1000);
        disable_air_positive();
    }


    while (debug == 0) {
        // Precharge
        if (sdc_present() && state == HVC_STANDBY) {
            // simple_precharge();
            debug_print("\n#######################################\n");
            debug_print("#     STARTING PRECHARGE SEQUENCE     #\n");
            debug_print("#######################################\n");

            state = active_precharge();
            // state = simple_precharge();
            
            debug_print("\n#######################################\n");
            debug_print("#     PRECHARGE SEQUENCE COMPLETE     #\n");
            debug_print("#######################################\n\n");

            debug_print_hvc_state(state);
        }
        // Check for SDC   
        if (!sdc_present() && state == HVC_TS_ENERGIZED) {
            // debug_print("[DEBUG] HVC STANDBY\n");
            disable_all_relays();
            state = HVC_STANDBY;
            debug_print("[DEBUG] SHUTDOWN CIRCUIT LOST...\n");
            debug_print_hvc_state(state);
        }
        // Status LED
        if (HAL_GetTick() - previousTick > interval ) {
            previousTick = HAL_GetTick();  
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Status LED
            debug_printf("[DEBUG] Tick: %lu\n", HAL_GetTick());
            debug_print_hvc_state(state);
            print_hv_adc_data();

        }        
    }
}


