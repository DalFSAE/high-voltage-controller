#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    RELAY_PC = 0,
    RELAY_AIR_N,
    RELAY_AIR_P,
    RELAY_NUM
} RelayId_t;

void relay_set(RelayId_t id, bool state);
void relay_all_off(void);