#include "debug_uart.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>


#define DEBUG_UART_BUFFER_SIZE 128

static UART_HandleTypeDef *debug_uart = NULL;
static char debug_buffer[DEBUG_UART_BUFFER_SIZE];

void debug_uart_init(UART_HandleTypeDef *huart) {
    debug_uart = huart;
}

void debug_print(const char *msg) {
    if (debug_uart) {
        HAL_UART_Transmit(debug_uart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }
}

void debug_printf(const char *fmt, ...) {
    if (debug_uart) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(debug_buffer, DEBUG_UART_BUFFER_SIZE, fmt, args);
        va_end(args);
        HAL_UART_Transmit(debug_uart, (uint8_t *)debug_buffer, strlen(debug_buffer), HAL_MAX_DELAY);
    }
}

void debug_print_float(float value, int decimal_places) {
    int int_part = (int)value;
    int frac_part;

    if (value < 0) {
        debug_printf("-");
        value = -value;
        int_part = -int_part;
    }

    frac_part = (int)((value - int_part) * pow(10, decimal_places));

    // Zero-padding for fractional part
    debug_printf("%d.%0*d", int_part, decimal_places, frac_part);
}
