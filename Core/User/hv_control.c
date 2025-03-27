#include "hv_control.h"

#include "hvc_config.h"
#include "adc_driver.h"
#include "debug_uart.h"

#include "stm32g0xx_hal.h"


void enable_precharge_relay(){
    debug_print("[DEBUG] Precharge Relay Enabled\n");
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_SET);
}

void enable_air_negative(){
    debug_print("[DEBUG] Negative AIR Enabled\n");
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_SET);
}

void enable_air_positive(){
    debug_print("[DEBUG] Positive AIR Enabled\n");
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_SET);
}

void disable_precharge_relay(){
    debug_print("[DEBUG] Precharge  Relay Disabled\n");
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_RESET);
}

void disable_air_negative(){
    debug_print("[DEBUG] Negative AIR Disabled\n");
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_RESET);
}

void disable_air_positive(){
    debug_print("[DEBUG] Positive AIR Disabled\n");
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_RESET);
}

void disable_all_relays(){
    debug_print("[DEBUG] All Relays Disabled\n");
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_RESET);
}




float amc1301_adc_to_voltage(uint16_t adc_val) {
    return ((float)adc_val - 76.0f) * (30.0f / 247.0f);
}

float measure_pack_voltage() {
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        // Handle the ADC timeout error, for example by returning an error value.
        return -1.0f; 
   }

   float voltage = amc1301_adc_to_voltage(adc_buf[ADC_VBATT]);
   
   debug_printf("[DEBUG] Pack Voltage (V):      ");
   debug_print_float(voltage, 2);
   debug_print("\n");

   return voltage;
}

float measure_ts_voltage() {
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        // Handle the ADC timeout error accordingly.
        return -1.0f;
   }

   float voltage = amc1301_adc_to_voltage(adc_buf[ADC_VTS]);

   debug_printf("[DEBUG] TS Voltage (V):        ");
   debug_print_float(voltage, 2);
   debug_print("\n");

   return voltage;
}

float measure_cur_ch1() {
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        // Handle the ADC timeout error accordingly.
        return -1.0f;
   }
    // inverse of y = 54.282x + 1808.8, based on testing data
   float current = ((float)adc_buf[ADC_HALL_CH1] - 2138.5f) / 54.282f; 
   debug_printf("[DEBUG] HALL CH1 Current (A):  ");
   debug_print_float(current, 2);
   debug_print("\n");
   
   return current;
}

float measure_cur_ch2() {
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        // Handle the ADC timeout error accordingly.
        return -1.0f;
   }

   float current = ((float)adc_buf[ADC_HALL_CH1] - 2138.5f) / 54.282f; // NOT VERIFED WITH HIGH CURRENT SUPPLY
   debug_printf("[DEBUG] HALL CH2 Current (A):  ");
   debug_print_float(current, 2);
   debug_print("\n");

   return current;
}

void print_hv_adc_data() {
    measure_pack_voltage();
    measure_ts_voltage();
    measure_cur_ch1();
    measure_cur_ch2();
    debug_print("\n");

}

bool sdc_present() {
    if (!adc_wait_for_conversion(ADC_DEFAULT_TIMEOUT_MS)) {
        return false; // ADC Timeout
    }
    return (adc_buf[ADC_SDC_IN] >= SDC_ADC_MIN_VOLTAGE);
}

void debug_print_hvc_state(HVC_State_t state) {
    switch (state) {
        case HVC_OFF:
            debug_print("[STATE] OFF\n");
            break;
        case HVC_STANDBY:
            debug_print("[STATE] STANDBY\n");
            break;
        case HVC_PC_ACTIVE:
            debug_print("[STATE] PRECHARGE ACTIVE\n");
            break;
        case HVC_TS_ENERGIZED:
            debug_print("[STATE] TS ENERGIZED\n");
            break;
        case HVC_PRECHARGE_FAULT:
            debug_print("[STATE] PRECHARGE FAULT (SDC disconnected?)\n");
            break;
        case HVC_SDC_FAULT:
            debug_print("[STATE] SDC FAULT\n");
            break;
        case HVC_TIMEOUT_FAULT:
            debug_print("[STATE] PRECHARGE TIMEOUT\n");
            break;
        case HVC_MINTIME_VAULT:
            debug_print("[STATE] TS reached voltage too early\n");
            break;
        default:
            debug_printf("[STATE] UNKNOWN STATE: %d\n", state);
            break;
    }
}

HVC_State_t active_precharge() {

    debug_print("[DEBUG] Precharge Sequence Started\n");

    HVC_State_t hvcState = HVC_STANDBY;    

    if (!sdc_present()) {
        hvcState = HVC_PRECHARGE_FAULT;
        goto cleanup;
    }
    
    enable_air_negative(); // Close AIR- 

   HAL_Delay(100); // gives ADC time to measure pack voltage 

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
                hvcState = HVC_MINTIME_VAULT;
                goto cleanup;
            }
            else {
                enable_air_positive();
                disable_precharge_relay();
                hvcState = HVC_TS_ENERGIZED;
            }
        }
    }
    
cleanup:
    // disable all relays if any errors occurs
    if (hvcState != HVC_TS_ENERGIZED) {
        disable_all_relays();
        debug_printf("[DEBUG] PRECHARGE FAULT...\n");
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