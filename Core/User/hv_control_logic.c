#include "hv_control_logic.h"

void hvc_init(HvcContext *context, uint32_t now_ms) {
    context->state = HVC_INIT;
    context->start_tick_ms = now_ms;
    context->fault = HVC_FAULT_CLEAR;
    return;
}


void hvc_update(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    return;
}