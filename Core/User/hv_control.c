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


float amc1301_adc_to_voltage(uint16_t adc_val) {
    return ((float)adc_val - 76.0f) * (30.0f / 247.0f);
}

float measure_pack_voltage() {
    return amc1301_adc_to_voltage(adc_buf[ADC_VBATT]);
}

float measure_ts_voltage() {
    return amc1301_adc_to_voltage(adc_buf[ADC_VTS]);
}



bool sdc_present() {
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        return false; // ADC Timeout
    }
    return (adc_buf[ADC_SDC_IN] >= SDC_ADC_MIN_VOLTAGE);
}


HVC_State_t active_precharge() {
    HVC_State_t hvcState = HVC_STANDBY;    

    if (!sdc_present()) {
        hvcState = HVC_PRECHARGE_FAULT;
        goto cleanup;
    }

    
    enable_air_negative(); // Close AIR- 

    // Halt function and wait for ADC to update
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        hvcState = HVC_TIMEOUT_FAULT;
        goto cleanup;
    }

    float packVoltage = measure_pack_voltage();
    float tsVoltage = measure_ts_voltage(); // Should be close to 0

    // Begin precharge sequence 
    enable_precharge_relay(); 
    hvcState = HVC_PC_ACTIVE;
    
    // Monitor TS voltage until it reaches threshold% of the pack voltage.  
    uint32_t startTime = HAL_GetTick();
    while (hvcState == HVC_PC_ACTIVE) {
        // Check for timeout
        if ((HAL_GetTick() - startTime) > PC_TIMEOUT_MS) {
            hvcState = HVC_TIMEOUT_FAULT;
            goto cleanup;
        }

        // Check if 12V still present
        if (!sdc_present()) {
            hvcState = HVC_PRECHARGE_FAULT;
            goto cleanup;
        }

        // Read TS voltage 
        tsVoltage = measure_ts_voltage();
        if (tsVoltage >= PC_THRESH * packVoltage) {
            if ((HAL_GetTick() - startTime) < PC_MINTIME_MS) {
                hvcState = HVC_PRECHARGE_FAULT;
                goto cleanup;
            }
            else {
                enable_air_negative();
                disable_precharge_relay();
                hvcState = HVC_TS_ENERGIZED;
            }
        }
    }
    
cleanup:
    // disable all relays if any errors occurs
    if (hvcState != HVC_TS_ENERGIZED) {
        disable_all_relays();
    }
    return hvcState;
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
    HAL_Delay(SIMPLE_PC_LENGTH_MS);
    // Close AIR+
    enable_air_positive();
    // Open PC
    disable_precharge_relay();
    return HVC_TS_ENERGIZED;
}