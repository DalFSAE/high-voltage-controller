#include "hv_control_logic.h"

void hvc_s_init(HvcContext *context, HvcInputs *in, HvcOutputs *out);
void hvc_s_standby(HvcContext *context, HvcInputs *in, HvcOutputs *out);
void hvc_s_pc_active(HvcContext *context, HvcInputs *in, HvcOutputs *out);
void hvc_s_ts_energized(HvcContext *context, HvcInputs *in, HvcOutputs *out);
void hvc_s_fault(HvcContext *context, HvcInputs *in, HvcOutputs *out);



void hvc_init(HvcContext *context, uint32_t now_ms) {
    context->state = HVC_S_INIT;
    context->start_tick_ms = now_ms;
    context->fault = HVC_FAULT_CLEAR;
    return;
}

// Main HVC state machine update function
void hvc_update(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    
    switch (context->state) {
        case HVC_S_INIT:
            hvc_s_init(context, in, out);
            break;
        case HVC_S_STANDBY:
            hvc_s_standby(context, in, out);
            break;
        case HVC_S_PC_ACTIVE:
            hvc_s_pc_active(context, in, out);
            break;
        case HVC_S_TS_ENERGIZED:
            hvc_s_ts_energized(context, in, out);
            break;
        case HVC_S_FAULT:
            hvc_s_fault(context, in, out);
            break;
        default:
            context->state = HVC_S_FAULT;
    }
    return;
}

void hvc_s_init(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    // In INIT state, ensure all outputs are off
    out->air_n_on = false;
    out->pc_on = false;
    out->air_p_on = false;

    // Transition to STANDBY after initialization
    context->state = HVC_S_STANDBY;
}       

void hvc_s_standby(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    return;
}

void hvc_s_pc_active(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    return;
}

void hvc_s_ts_energized(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    return;
}

void hvc_s_fault(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    return;
}