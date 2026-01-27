#pragma once
#include <stdint.h>
#include <stdbool.h>

// HVC State Machine 
typedef enum {
    HVC_S_INIT                = 0x00U,
    HVC_S_STANDBY             = 0x01U,
    HVC_S_TS_MEASURE          = 0x02U,
    HVC_S_PC_ACTIVE           = 0x03U,
    HVC_S_TS_ENERGIZED        = 0x04U,
    HVC_S_FAULT               = 0x05U,
} HvcState_t;

typedef enum {
    HVC_FAULT_CLEAR = 0,
    HVC_FAULT_PC_TIMEOUT,
    HVC_FAULT_PC_TO_FAST,
    HVC_FAULT_OVER_VOLT,
    HVC_FAULT_UNDER_VOLT,
    HVC_FAULT_MEASURE_TIMEOUT,
} HvcFaultCode_t;

typedef enum {
    HVC_CMD_ENABLE_TS = 0,
    HVC_CMD_DISABLE_TS,
    HVC_CMD_CLEAR_FAULT,
} HvcCmd_t;

// Used to control PC state machine
typedef struct {
    HvcCmd_t command; 

    float tractive_v;
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


// Setter
static inline HvcInputs _hvc_inputs_default(void)
{
    HvcInputs in = {0};

    in.command = HVC_CMD_DISABLE_TS;
    in.imd_ok = false;
    in.bms_ok = false;
    in.sdc_ok = false;
    in.tractive_v = 0.0f;
    in.battery_v = 0.0f;
    in.now_ms = 0;

    return in;
}
