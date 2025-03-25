#include "stm32g0xx_hal.h"
#include "main.h"
#include "dma.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"

#include <stdlib.h>
#include"hvc_config.h"

// CANbus handle
extern FDCAN_HandleTypeDef hfdcan1;

// CANbus Heartbeat 
FDCAN_TxHeaderTypeDef TxHeartbeat;
uint8_t TxHeartbeatData[8];




void can_heartbeat(){
    
    // Transmit Heartbeat   
    TxHeartbeatData[0] = 0xFF;
    TxHeartbeatData[1] = 0xFF;
    TxHeartbeatData[2] = 0xFF;
    TxHeartbeatData[3] = 0xFF;
    TxHeartbeatData[4] = 0xFF;
    TxHeartbeatData[5] = 0xFF;
    TxHeartbeatData[6] = 0xFF;
    TxHeartbeatData[7] = 0xFF;

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeartbeat, TxHeartbeatData) != HAL_OK)
    {
        /* Transmission request Error */
        Error_Handler();
    }
}


void HeartbeatTaskEntry(void *argument){
    (void)argument;
    
    // Heartbeat Signal 
    TxHeartbeat.Identifier = 0x320;
    TxHeartbeat.IdType = FDCAN_STANDARD_ID;
    TxHeartbeat.TxFrameType = FDCAN_DATA_FRAME;
    TxHeartbeat.DataLength = FDCAN_DLC_BYTES_8;
    TxHeartbeat.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeartbeat.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeartbeat.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeartbeat.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeartbeat.MessageMarker = 0;

    for (;;){
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
        can_heartbeat();
        osDelay(HEARTBEAT_TIME);
    }
}