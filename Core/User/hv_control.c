#include "hv_control.h"

#include "hvc_config.h"
#include "adc_driver.h"

#include "stm32g0xx_hal.h"


void enable_precharge_relay(){
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_SET);
}

void enable_air_negative(){
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_SET);
}

void enable_air_positive(){
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_SET);
}

void disable_precharge_relay(){
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_RESET);
}

void disable_air_negative(){
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_RESET);
}

void disable_air_positive(){
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_RESET);
}

void disable_all_relays(){
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_RESET);
}




bool sdc_present() {

    

    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        return false; // ADC Timeout
    }
    return (adc_buf[ADC_SDC_IN] >= SDC_ADC_MIN_VOLTAGE);
}


HVC_State_t active_precharge() {
    HVC_State_t result = HVC_STANDBY;    

    if (!sdc_present()) {
        result = HVC_PRECHARGE_FAULT;
        goto cleanup;
    }

    // Close AIR- and measure pack voltage
    enable_air_negative();
    
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        result = HVC_PRECHARGE_FAULT;
        goto cleanup;
    }
    
    
    
    
    // Precharge feedback loop
    
    uint32_t previousTick = 0;
    uint32_t interval = 500; // Interval in milliseconds
    
    if (HAL_GetTick() - previousTick > interval ) {
        previousTick = HAL_GetTick();  
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12); // Status LED
    }
    
cleanup:
    // disable all relays if any errors occurs
    if (result != HVC_TS_ENERGIZED) {
        disable_all_relays();
    }
    return result;
}

HVC_State_t simple_precharge() {

    // Check if SDC present
    if (!sdc_present()) {
        return false;
    }

    // Close AIR-
    enable_air_negative();
    HAL_Delay(100);
    // Close PC
    enable_precharge_relay();
    // Wait for precharge 
    HAL_Delay(PRECHARGE_LENGTH_MS);
    // Close AIR+
    enable_air_positive();
    // Open PC
    disable_precharge_relay();
    return HVC_TS_ENERGIZED;
}