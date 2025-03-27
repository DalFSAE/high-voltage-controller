#ifndef DEBUG_UART_H
#define DEBUG_UART_H

#include "stm32g0xx_hal.h"


// Initialize debug UART (optional, only if needed)
void debug_uart_init(UART_HandleTypeDef *huart);

// Send a null-terminated debug message over UART
void debug_print(const char *msg);

// Send a formatted debug message like printf
void debug_printf(const char *fmt, ...);

void debug_print_float(float value, int decimal_places);

#endif // DEBUG_UART_H
