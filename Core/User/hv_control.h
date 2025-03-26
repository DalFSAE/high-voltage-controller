#ifndef HV_CONTROL_H
#define HV_CONTROL_H

#include <stdbool.h>

bool sdc_present(void);
bool simple_precharge(void);

// bool asyn_precharge(void);


// HVC State Machine 
typedef enum {
    HVC_OFF                 = 0x00U,
    HVC_STANDBY             = 0x01U,
    HVC_PRECHARGE           = 0x02U,
    HVC_TS_ENERGIZED        = 0x03U,
    HVC_PRECHARGE_FAULT     = 0x04U,
    HVC_SDC_FAULT           = 0x05U,
    HVC_TIMEOUT_FAULT       = 0x06U
} HVC_State_t;


#endif