

// #include "stm32g0xx_hal.h"

// #include "app_main.h"
// #include "io_control.h"
// #include "hvc_config.h"

// #include "main.h"
// #include "dma.h"
// #include "FreeRTOS.h"
// #include "cmsis_os.h"

// #include <stdlib.h>
// #include <stdbool.h>
// #include <math.h>



// // Defines
// #define ADC_BUFFER_LEN 3


// // Global State Machine 
// HVC_State_t hvcState;

// // External Handles 
// extern ADC_HandleTypeDef hadc1;
// extern TIM_HandleTypeDef htim2; 


// // ADC Buffer
// volatile uint16_t adc_buf[ADC_BUFFER_LEN]; // NOTE MOVE TO VCU STYLE BUFFER

// // CANbus handle
// extern FDCAN_HandleTypeDef hfdcan1;

// // Globals for CAN
// volatile bool commandMessageReceived = false;
// volatile bool tractiveSystemEnableCmd = false;
// volatile bool sdcInStatus = true;   // TODO: Check GPIO pin and set this to default false
// uint32_t lastMessageTime = 0;       // 


// static void FDCAN_Config(void) {
//     FDCAN_FilterTypeDef sFilterConfig;

//     // Configure Rx filter 
//     sFilterConfig.IdType = FDCAN_STANDARD_ID;
//     sFilterConfig.FilterIndex = 0;
//     sFilterConfig.FilterType = FDCAN_FILTER_MASK;
//     sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // FDCAN_FILTER_TO_RXFIFO0
//     sFilterConfig.FilterID1 = 0x000;
//     sFilterConfig.FilterID2 = 0x000;
//     if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
//     {
//         Error_Handler();
//     }

//     //     Configure global filter:
//     //     Filter all remote frames with STD and EXT ID
//     //     Reject non matching frames with STD ID and EXT ID 
//     if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
//     {
//         Error_Handler();
//     }

//     .. Start the FDCAN module 
//     if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
//     {
//         Error_Handler();
//     }

//     if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
//     { 
//         Error_Handler();
//     }
// }

// // TODO: Rework to not use floats?
// float percentDif(int value1, int value2){ 
//     if (value1 == 0 && value2 == 0){
//         return 0.0;
//     }

//     float result = fabs((float)value1 - (float)value2) / (((float)abs(value1) + (float)abs(value2)) / 2.0) * 100.0;
//     return result;
// }

// uint32_t start_precharge(){
//     bool successStatus = false;
    

//     // Close AIR-
//     enable_air_negative();
    
//     // Check Battery Voltage
//     vTaskDelay(pdMS_TO_TICKS(500)); 
    

//     // Enable Precharge
//     enable_precharge_relay();
//     vTaskDelay(pdMS_TO_TICKS(500)); 

//     // Check voltage or timer
    
//     uint32_t prechargeStartTime = HAL_GetTick();
//     // Wait at least PRECHARGE_MIN_TIME, ignoring the percent difference
//     while ((HAL_GetTick() - prechargeStartTime) < PRECHARGE_MIN_TIME){
//         // Wait 
//     }
//     uint32_t vbatt = adc_buf[0];
//     // After minimum time has passed, check percent difference and maximum time 
//     do {
//         // Timeout if it takes too long
//         if ((HAL_GetTick() - prechargeStartTime) >= PRECHARGE_MAX_TIME){
//             goto cleanup;
//         }
//     } while (percentDif(vbatt, adc_buf[1]) >= PRECHARGE_PERCENT_DIF);


//     if (0) { // TODO: Write code to time how long precharge takes
//         goto cleanup;
//     }

//     // Enable AIR +
//     enable_air_positive();
//     vTaskDelay(pdMS_TO_TICKS(100)); 

//     successStatus = true;

// cleanup:
//     disable_precharge_relay();

//     // Error Handling 
//     if (!successStatus){
//         disable_all_relays();
//     }
    
//     return successStatus;
// }

// void app_init(){

//     FDCAN_Config();

//     HAL_TIM_Base_Start(&htim2);
//     HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, ADC_BUFFER_LEN);
    
//     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
//     return;
// }


// void StateMachineTaskEntry(void *argument){
//     (void)argument;

//     hvcState = HVC_OFF;
//     for (;;) {

//         if ((HAL_GetTick() - lastMessageTime) > CMD_MSG_TIMEOUT) { // If no message received 
//             hvcState = HVC_TIMEOUT_FAULT;
//         }


//         // State Machine Logic 
//         switch (hvcState) {
//             case HVC_OFF:
//                 // Startup Logic
//                 hvcState = HVC_STANDBY;
//                 break;
            
//             case HVC_STANDBY:
//                 disable_all_relays();
//                 if (tractiveSystemEnableCmd) {
//                     hvcState = HVC_PRECHARGE;   // Transition state
//                 }
//                 break;

//             case HVC_PRECHARGE:
//                 if (start_precharge()) {
//                     hvcState = HVC_TS_ENERGIZED;
//                 } 
//                 else {
//                     hvcState = HVC_PRECHARGE_FAULT;
//                 }
//                 break;

//             case HVC_TS_ENERGIZED:
//                 if (tractiveSystemEnableCmd == false){
//                     disable_all_relays();
//                     hvcState = HVC_STANDBY;
//                 }
//                 break;

//             case HVC_PRECHARGE_FAULT:
//                 disable_all_relays();
//                 if (tractiveSystemEnableCmd == false){
//                     hvcState = HVC_STANDBY;
//                 }
//                 break;

//             case HVC_SDC_FAULT:
//                 disable_all_relays();
//                 if (tractiveSystemEnableCmd == false){
//                     hvcState = HVC_STANDBY;
//                 }
//                 break;

//             case HVC_TIMEOUT_FAULT:
//                 disable_all_relays();
//                 if (tractiveSystemEnableCmd == false){
//                     hvcState = HVC_STANDBY;
//                 }
//                 break;
            
//             default:
//                 break;
//         }
//         osDelay(10);
//     }
// }


// void HVCStatusTaskEntry(void *argument){
//     (void)argument;

//     FDCAN_TxHeaderTypeDef TxHeader; // HVC_Status_Message 
//     uint8_t TxData[7];

//     //  Prepare Tx Header 
//     TxHeader.Identifier = 0x01B;
//     TxHeader.IdType = FDCAN_STANDARD_ID;
//     TxHeader.TxFrameType = FDCAN_DATA_FRAME;
//     TxHeader.DataLength = FDCAN_DLC_BYTES_8;
//     TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
//     TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
//     TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
//     TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
//     TxHeader.MessageMarker = 0;

//     for(;;){
        
//         TxData[0] = (uint8_t)hvcState;
//         // VBATT
//         uint16_t vbatt = adc_buf[0];        
//         TxData[1] = (uint8_t)(vbatt & 0xFF);
//         TxData[2] = (uint8_t)((vbatt >> 8) & 0xFF);
                 
//         // VTS
//         uint16_t vts = adc_buf[1];
//         TxData[3] = (uint8_t)(vts & 0xFF);
//         TxData[4] = (uint8_t)((vts >> 8) & 0xFF); 
        

//         if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, TxData) != HAL_OK)
//         {
//             //  Transmission request Error
//             Error_Handler();
//         }


//         osDelay(500);
//     }
// }


// void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc) {
// 	(void)hadc;
// }

// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
//     // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
//     (void)hadc;
// }

// void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
// {
//     FDCAN_RxHeaderTypeDef RxHeader;
//     uint8_t RxData[8];
    
//     if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
//         {
//         //  retrieve Rx messages from RX FIFO0 
//         if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
//             // /* Reception Error 
//             Error_Handler();
//         }
        
//         if ((RxHeader.Identifier == 0x1A) && (RxHeader.IdType == FDCAN_STANDARD_ID)) {
//             if (RxData[0] == 1){ // TODO: Add SDC check
//                 if (sdcInStatus == true){ 
//                     tractiveSystemEnableCmd = true; 
//                 }
//                 else {
//                     hvcState = HVC_SDC_FAULT; // Attempted to enable TS, while SDC is false
//                 }
//             }
//             else if (RxData[0] == 0) {
//                 tractiveSystemEnableCmd = false; 
//             }
//             lastMessageTime = HAL_GetTick();
//         }

//         if ((RxHeader.Identifier == 0x321) && (RxHeader.IdType == FDCAN_STANDARD_ID)) {
//             HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
//         }
//     }
// }
