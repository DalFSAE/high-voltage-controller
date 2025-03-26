 #ifndef HVC_CONFIG_H
 #define HVC_CONFIG_H

#define HEARTBEAT_TIME 1000
#define CMD_MSG_TIMEOUT 1000

#define PRECHARGE_MIN_TIME 1000
#define PRECHARGE_MAX_TIME 10000
#define PRECHARGE_PERCENT_DIF 5.0f

#define AIR_P_PORT GPIOC
#define AIR_M_PORT GPIOC
#define PC_PORT GPIOC

#define AIR_P_PIN GPIO_PIN_2
#define AIR_M_PIN GPIO_PIN_1
#define PC_PIN GPIO_PIN_0

#define ADC_BUFFER_LEN 8

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


// PRECHARGE SETTINGS

#define SDC_ADC_MIN_VOLTAGE 128
#define PRECHARGE_LENGTH_MS 5000 
#define PRECHARGE_TIMEOUT_MS 10000


#endif