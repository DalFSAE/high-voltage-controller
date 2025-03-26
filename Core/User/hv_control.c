#include "hv_control.h"
#include "io_control.h"
#include "hvc_config.h"


#include "stm32g0xx_hal.h"

extern volatile uint16_t adc_buf[ADC_BUFFER_LEN];

bool sdc_present() {
    if (adc_buf[ADC_SDC_IN] >= SDC_ADC_MIN_VOLTAGE) {
        return true;
    }
    return false;
}

bool simple_precharge() {

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
    return true;
}