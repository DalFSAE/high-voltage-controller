#include "can_driver.h"
#include "fdcan.h"
#include "string.h"          /* memcpy  */
#include "stm32g0xx_hal.h"   /* HAL_FDCAN */

extern FDCAN_HandleTypeDef hfdcan1;   /* created by CubeMX (standard CAN 500 kbit/s) */

/* ─────────  Helpers  ───────── */
static void tx_std(uint16_t id, const void *data, uint8_t dlc);

static FDCAN_TxHeaderTypeDef hdr = {
    .IdType             = FDCAN_STANDARD_ID,
    .TxFrameType        = FDCAN_DATA_FRAME,
    .ErrorStateIndicator= FDCAN_ESI_ACTIVE,
    .BitRateSwitch      = FDCAN_BRS_OFF,
    .FDFormat           = FDCAN_CLASSIC_CAN,
    .TxEventFifoControl = FDCAN_NO_TX_EVENTS,
    .MessageMarker      = 0
};

/* ─────────  Public functions  ───────── */

void can_init(void)
{
    /* Start peripheral */
    HAL_FDCAN_Start(&hfdcan1);

    /* Global filter: accept everything (change when Rx filters are added) */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
        FDCAN_FILTER_DISABLE,  /* std  */
        FDCAN_FILTER_DISABLE,  /* ext  */
        FDCAN_REJECT_REMOTE,
        FDCAN_REJECT_REMOTE);

    /* Optionally send version / build tag once at boot                  */
    const char tag[8] = "HVC204C";      /* ≤ 8 ASCII chars               */
    tx_std(CAN_ID_HVC_VERSION, tag, 8); /* DLC-8                         */
}

/* ---------- 10-ms heartbeat ---------- */
void can_task_10ms(bool imd_ok, bool bms_ok, uint8_t hvc_state)
{
    uint8_t buf[3] = { hvc_state,
                       imd_ok ? 1u : 0u,
                       bms_ok ? 1u : 0u };
    tx_std(CAN_ID_HVC_STATUS, buf, 3);
}

/* ---------- 50-ms slow data ---------- */
void can_task_50ms(float pack_V, float ts_V,
                   float I_ch1_A, float I_ch2_A)
{
    /* Voltages (0.1 V resolution) */
    uint16_t v_pack = (uint16_t)(pack_V * 10.0f);
    uint16_t v_ts   = (uint16_t)(ts_V   * 10.0f);
    uint8_t  volt[4];
    memcpy(volt,     &v_pack, 2);
    memcpy(volt + 2, &v_ts,   2);
    tx_std(CAN_ID_HV_VOLTAGES, volt, 4);

    /* Currents (0.1 A, signed) */
    int16_t i1 = (int16_t)(I_ch1_A * 10.0f);
    int16_t i2 = (int16_t)(I_ch2_A * 10.0f);
    uint8_t  curr[4];
    memcpy(curr,     &i1, 2);
    memcpy(curr + 2, &i2, 2);
    tx_std(CAN_ID_HV_CURRENTS, curr, 4);
}

/* ---------- Pre-charge diagnostic (event driven) ---------- */
void can_send_pc_diag(uint8_t reason, float ts_V_at_abort)
{
    uint16_t ts_dV = (uint16_t)(ts_V_at_abort * 10.0f);
    uint8_t  buf[3];
    buf[0] = reason;
    memcpy(buf + 1, &ts_dV, 2);
    tx_std(CAN_ID_PC_DIAG, buf, 3);
}

/* ─────────  Internal transmit helper  ───────── */
static void tx_std(uint16_t id, const void *data, uint8_t len)
{
    hdr.Identifier = id;
    /* CAN DLC table: 0,1,2,3,4,5,6,7,8 bytes → FDCAN_DLC_BYTES_n  */
    static const uint8_t dlc_lut[9] = {
        FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2,
        FDCAN_DLC_BYTES_3, FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5,
        FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7, FDCAN_DLC_BYTES_8 };
    hdr.DataLength = dlc_lut[len];

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &hdr, (uint8_t *)data);
}
