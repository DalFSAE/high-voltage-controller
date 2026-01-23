#include "relay.h"
#include "hvc_config.h" 

#include "stm32g0xx_hal.h"

void relay_set(RelayId_t id, bool state) {
    switch (id)
    {
    case RELAY_PC:
        HAL_GPIO_WritePin(PC_PORT, PC_PIN, state);
        break;
    case RELAY_AIR_P:
        HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, state);
        break;
    case RELAY_AIR_N:
        HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, state);
        break;
    default:
        break;
    }    
    return;
}
void relay_all_off(void) {
    for (int i = 0; i < RELAY_NUM; i++) {
        relay_set(i, false);
    }
    return;
}