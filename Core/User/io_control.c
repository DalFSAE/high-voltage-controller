#include "io_control.h"
#include "hvc_config.h"

#include "stm32g0xx_hal.h"


void enable_precharge_relay(){
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_SET);
}

void enable_air_negative(){
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_SET);
}

void enable_air_positive(){
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_SET);
}

void disable_precharge_relay(){
    HAL_GPIO_WritePin(PC_PORT, PC_PIN, GPIO_PIN_RESET);
}

void disable_air_negative(){
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_RESET);
}

void disable_air_positive(){
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_RESET);
}

void disable_all_relays(){
    HAL_GPIO_WritePin(PC_PIN, PC_PORT, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_M_PORT, AIR_M_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIR_P_PORT, AIR_P_PIN, GPIO_PIN_RESET);
}

