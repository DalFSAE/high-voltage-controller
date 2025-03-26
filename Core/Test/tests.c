
#include "tests.h"
#include "hvc_config.h"
#include "hv_control.h"

#include "stm32g0xx_hal.h"
#include "core_cm0plus.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern TIM_HandleTypeDef htim2; 

volatile uint16_t adc_buf[ADC_BUFFER_LEN];

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
    bool debug = false;

    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUFFER_LEN);
    HAL_TIM_Base_Start(&htim2);
    
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
            state = HVC_TS_ENERGIZED;
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


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc1) 
{
     // Process the data
     HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_11); // Status LED
}