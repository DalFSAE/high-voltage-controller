#ifndef HV_CONTROL_H
#define HV_CONTROL_H

#include <stdbool.h>

// HVC State Machine 
typedef enum {
    HVC_OFF                 = 0x00U,
    HVC_STANDBY             = 0x01U,
    HVC_PC_ACTIVE           = 0x02U,
    HVC_TS_ENERGIZED        = 0x03U,
    HVC_PRECHARGE_FAULT     = 0x04U,
    HVC_SDC_FAULT           = 0x05U,
    HVC_TIMEOUT_FAULT       = 0x06U,
    HVC_MINTIME_VAULT       = 0x7U
} HVC_State_t;


bool sdc_present(void);
HVC_State_t simple_precharge(void);
HVC_State_t active_precharge(void);


void enable_precharge_relay();
void enable_air_negative();
void enable_air_positive();

void disable_precharge_relay();
void disable_air_negative();
void disable_air_positive();

void disable_all_relays();


// bool asyn_precharge(void);





#endif