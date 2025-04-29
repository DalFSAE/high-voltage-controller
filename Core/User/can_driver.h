#pragma once
#include "stdbool.h"
#include "stdint.h"

/* ─────────  CAN-IDs (11-bit, standard)  ───────── */
#define CAN_ID_HVC_STATUS   0x201U   /* Heartbeat / overall state      */
#define CAN_ID_HV_CURRENTS  0x203U   /* HV currents (2 channels)       */
#define CAN_ID_PC_DIAG      0x204U   /* Pre-charge diagnostic (on err) */
#define CAN_ID_HVC_VERSION  0x205U   /* Firmware build tag (boot only) */
#define CAN_ID_HV_VOLTAGES  0x206U   /* Pack + TS voltages             */

/* ─────────  Public API  ───────── */
void can_init(void);

/* Call every 10 ms  */
void can_task_10ms(bool imd_ok, bool bms_ok, uint8_t hvc_state);

/* Call every 50 ms  */
void can_task_50ms(float pack_V, float ts_V,
                   float I_ch1_A, float I_ch2_A);

/* Optional: call on pre-charge fault */
void can_send_pc_diag(uint8_t reason, float ts_V_at_abort);