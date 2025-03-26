#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H


#include "stm32g0xx_hal.h"
#include "adc.h"
#include <stdbool.h>
#include <stdint.h>



#define ADC_BUFFER_LEN 8
#define ADC_DEFAULT_TIMEOUT_MS 5000

// Expose the ADC buffer for other modules to access
extern volatile uint16_t adc_buf[ADC_BUFFER_LEN];

typedef enum {
    ADC_VBATT,
    ADC_VTS,
    ADC_HALL_CH1,
    ADC_HALL_CH2,
    ADC_IMON,
    ADC_SDC_IN,
    ADC_SDC_LATCH,
    ADC_MPU_TEMP,
    ADC_NUM_SENSORS  // This is optional, but useful to denote the number of sensors
} SensorType_t;

void adc_init(void);

bool adc_wait_for_conversion(uint32_t timeout_ms);

#endif