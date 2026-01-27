#include "hv_control_logic.h"
#include "hv_control.h"
#include "hvc_config.h"

#include <stdbool.h>

static bool check_sdc(HvcContext *context, HvcInputs *in, HvcOutputs *out);
static void hvc_s_init(HvcContext *context, HvcInputs *in, HvcOutputs *out);
static void hvc_s_standby(HvcContext *context, HvcInputs *in, HvcOutputs *out);
static void hvc_s_measure(HvcContext *context, HvcInputs  *in, HvcOutputs *out);
static void hvc_s_pc_active(HvcContext *context, HvcInputs *in, HvcOutputs *out);
static void hvc_s_ts_energized(HvcContext *context, HvcInputs *in, HvcOutputs *out);
static void hvc_s_fault(HvcContext *context, HvcInputs *in, HvcOutputs *out);

void hvc_init(HvcContext *context, uint32_t now_ms) {
    context->state = HVC_S_INIT;
    context->start_tick_ms = now_ms;
    context->fault = HVC_FAULT_CLEAR;
    return;
}

// Main HVC state machine update function
void hvc_update(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    // state functions will enable required outputs each tick
    out->air_n_on = false;
    out->air_p_on = false;
    out->pc_on    = false;

    // SDC may open at any time, for many reasons
    // we can recover if the SDC signal returns
    if (!check_sdc(context, in, out)) {
        return;
    }

    // HVC State Machine
    switch (context->state) {
        case HVC_S_INIT:
            hvc_s_init(context, in, out);
            break;
        case HVC_S_STANDBY:
            hvc_s_standby(context, in, out);
            break;
        case HVC_S_TS_MEASURE:
           hvc_s_measure(context, in, out);
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
}

static inline void hvc_bail_out(HvcContext *context, HvcOutputs *out, HvcState_t next_state) {
    // Immediately force safe outputs
    out->air_n_on = false;
    out->air_p_on = false;
    out->pc_on    = false;

    context->state = next_state;
}

// Entry state
static void hvc_s_init(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    out->air_n_on = false;
    out->pc_on = false;
    out->air_p_on = false;

    // Transition to STANDBY after initialization
    context->state = HVC_S_STANDBY;
}

// SDC is present, but we are waiting for the PC command.
static void hvc_s_standby(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    out->air_n_on = false;
    out->air_p_on = false;
    out->pc_on    = false;

    if (in->command == HVC_CMD_ENABLE_TS) {
        out->air_n_on = true;                   // close AIR_N to read TS voltage
        context->start_tick_ms = in->now_ms;    // store time for timeout checks
        context->state = HVC_S_TS_MEASURE;      // proceede
    }
}

// We must close AIR_N, in order to measure the battery voltage
static void hvc_s_measure(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    out->air_n_on = true;
    out->air_p_on = false;
    out->pc_on    = false;

    if (in->command != HVC_CMD_ENABLE_TS) {
        hvc_bail_out(context, out, HVC_S_STANDBY);
        return;
    }
 
    uint32_t dt = in->now_ms - context->start_tick_ms;
 
    // wait of TS voltage to settle
    if (dt < PC_SETTLE_MS) {
        return;
    }

    if (dt >= PC_MEASURE_TIMEOUT_MS) {
        context->fault = HVC_FAULT_MEASURE_TIMEOUT;
        hvc_bail_out(context, out, HVC_S_FAULT);
        return;
    }

    if (in->battery_v < V_BAT_MIN) {
        context->fault = HVC_FAULT_UNDER_VOLT;
        hvc_bail_out(context, out, HVC_S_FAULT);
        return;
    }

    if (in->battery_v > V_BAT_MAX) {
        context->fault = HVC_FAULT_OVER_VOLT;
        hvc_bail_out(context, out, HVC_S_FAULT);
        return;
    }

    // we should now have a stable voltage reading
    context->start_tick_ms = in->now_ms;    // store time for timeout checks
    context->state = HVC_S_PC_ACTIVE;
}

// Now we are precharging!
static void hvc_s_pc_active(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    out->air_n_on = true;
    out->air_p_on = false;
    out->pc_on    = true;

    if (in->command != HVC_CMD_ENABLE_TS) {
        hvc_bail_out(context, out, HVC_S_STANDBY);
        return;
    }

    uint32_t dt = in->now_ms - context->start_tick_ms;

    // often caused by a missing precharge resistor
    if (dt >= PRECHARGE_MAX_TIME) {
        context->fault = HVC_FAULT_PC_TIMEOUT;
        hvc_bail_out(context, out, HVC_S_FAULT);
        return;
    }

    if (in->tractive_v >= PC_THRESH * in->battery_v) {
        // something is wrong if we precharge *too* fast.
        if(dt <= PC_MINTIME_MS) {
            context->fault = HVC_FAULT_PC_TO_FAST;
            hvc_bail_out(context, out, HVC_S_FAULT);
            return;
        }
        // its alive!
        else {
            context->start_tick_ms = in->now_ms;
            context->state = HVC_S_TS_ENERGIZED;
        }
    }
}

static void hvc_s_ts_energized(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    out->air_n_on = true;
    out->air_p_on = true;
    out->pc_on    = false;

    if (in->command != HVC_CMD_ENABLE_TS) {
        hvc_bail_out(context, out, HVC_S_STANDBY);
        return;
    }

    uint32_t dt = in->now_ms - context->start_tick_ms;
 
    // max 100ms of overlap allowed
    // this helps avoid any gap
    if (dt < PC_OVERLAP_MS) {
        out->pc_on = true;
    }
}

static void hvc_s_fault(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    out->air_n_on = false;
    out->air_p_on = false;
    out->pc_on    = false;

    if (in->command == HVC_CMD_CLEAR_FAULT) {
        context->fault = HVC_FAULT_CLEAR;
        context->state = HVC_S_STANDBY;
        return;
    }

}

static bool check_sdc(HvcContext *context, HvcInputs *in, HvcOutputs *out) {
    if (in->sdc_ok) {
        return true;
    }

    // SDC is open, force safe outputs
    out->air_n_on = false;
    out->air_p_on = false;
    out->pc_on    = false;

    // we can recover from a SDC fault, so return to standby
    context->fault = HVC_FAULT_SDC_LOST;
    context->state = HVC_S_STANDBY;
    return false;
}

