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


// PRECHARGE SETTINGS

#define SDC_ADC_MIN_VOLTAGE 128
#define SIMPLE_PC_LENGTH_MS 5000    // 
#define PC_TIMEOUT_MS 20000 // Throw a fault if PC is longer than value (ms)
#define PC_MINTIME_MS 2000  // Throw a fault if PC is shorter than value (ms)    
#define PC_THRESH 0.60f             // Bring TS to 95% of the pack voltage

#endif