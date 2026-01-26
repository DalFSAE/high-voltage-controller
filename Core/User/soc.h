#pragma once 

#include <stdint.h>
#include "integrator.h"


typedef struct {
    float x;
    uint32_t last_ms;
} Soc_t;

