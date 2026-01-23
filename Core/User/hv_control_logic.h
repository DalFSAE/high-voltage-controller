#pragma once
#include <stdint.h>

// HVC State Machine 
typedef enum {
    HVC_INIT                = 0x00U,
    HVC_STANDBY             = 0x01U,
    HVC_PC_ACTIVE           = 0x02U,
    HVC_TS_ENERGIZED        = 0x03U,
    HVC_SDC_FAULT           = 0x04U,
} HvcState_t;

typedef enum {
    HVC_FAULT_CLEAR = 0,
    HVC_FAULT_PC_TIMEOUT,
    HVC_FAULT_PC_TO_FAST,
    HVC_FAULT_OVER_VOLT,
    HVC_FAULT_UNDER_VOLT
} HvcFaultCode_t

typedef enum {
    ENABLE_TS = 0,
    DISABLE_TS,
    CLEAR_FAULT,
} HvcCmd_t;

// Used to control PC state machine
typedef struct {
   HvcCmd_t command; 

    float pack_v;
    float battery_v;

    bool imd_ok;
    bool bms_ok;
    bool sdc_ok;

    uint32_t now_ms;
} HvcInputs;

typedef struct {
    bool pc_on;
    bool air_p_on;
    bool air_n_on;
} HvcOutputs;

// This stores global context
typedef struct {
    uint32_t start_tick_ms;
    HvcState_t state;
    HvcFaultCode_t fault;
} HvcContext;

void hvc_init(HvcContext *context, uint32_t now_ms);
void hvc_update(HvcContext *context, HvcInputs *in, HvcOutputs *out);