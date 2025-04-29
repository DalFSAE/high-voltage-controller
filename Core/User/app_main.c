
#include "app_main.h"
#include "hvc_config.h"
#include "hv_control.h"
#include "adc_driver.h"
#include "debug_uart.h"
#include "can_driver.h"

#include "stm32g0xx_hal.h"
#include "core_cm0plus.h"
#include "main.h"

#include "debug_uart.h"

#include <stdio.h>

extern UART_HandleTypeDef huart5;



void app_init() {
    adc_init();
    debug_uart_init(&huart5);
    can_init();
    
    debug_print("#######################################\n");
    debug_print("# HIGH VOLTAGE CONTROLLER INITIALIZED #\n");
    debug_print("#######################################\n");
    
    enable_shutdown_circuit();
    return;
}

void app_main() {
    // bool debug = false;


    HVC_State_t state = HVC_STANDBY;

    uint32_t led_prev   = 0;      /* 1-s heartbeat  */
    uint32_t can10_prev = 0;      /* 10-ms CAN task */
    uint32_t can50_prev = 0;      /* 50-ms CAN task */

    const uint32_t led_interval   = 1000; /* ms */
    const uint32_t can10_interval =   10;
    const uint32_t can50_interval =   50;
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

        uint32_t now = HAL_GetTick();
        if (now - led_prev >= led_interval)
        {
            led_prev = now;
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);       /* status LED */
            debug_printf("[DEBUG] Tick: %lu\n", now);
            debug_print_hvc_state(state);
            print_hv_adc_data();
        }
                /* ───── 10-ms CAN heartbeat ───── */
        if (now - can10_prev >= can10_interval)
        {
            can10_prev = now;
            bool imd_ok = read_imd_status();
            bool bms_ok = read_bms_status();
            can_task_10ms(imd_ok, bms_ok, (uint8_t)state);
        }

        /* ───── 50-ms CAN slow signals ───── */
        if (now - can50_prev >= can50_interval)
        {
            can50_prev = now;
            can_task_50ms(measure_pack_voltage(),
                          measure_ts_voltage(),
                          measure_cur_ch1(),
                          measure_cur_ch2());
        }      
    }
}


