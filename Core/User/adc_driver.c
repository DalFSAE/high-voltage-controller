#include "adc_driver.h"
#include "stm32g0xx_hal.h"


extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2; 
extern DMA_HandleTypeDef hdma_adc1;

volatile uint16_t adc_buf[ADC_BUFFER_LEN];
static volatile bool adc_conversion_complete = false; // Must always reset after ues



void adc_init() {
    adc_conversion_complete = false;
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUFFER_LEN);
    HAL_TIM_Base_Start(&htim2);
    return;
}


bool adc_wait_for_conversion(uint32_t timeout_ms) {
    // Wait for ADC buffer callback to trigger
    uint32_t startTime = HAL_GetTick(); 
    while (!adc_conversion_complete) {
        if ((HAL_GetTick() - startTime) > timeout_ms) {
            return false;
        }
    }
    adc_conversion_complete = false; // reset flag
    return true;

}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc1) {
     // Process the data
     HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_11); // Status LED
     adc_conversion_complete = true;
}