#pragma once

#include <stdbool.h>
#include <stdint.h> 

typedef struct {
    float x;
    uint32_t last_ms;
} Integrator;

static inline void integrator_init(Integrator *i, uint32_t now_ms, float x0)
{
    i->x = x0;
    i->last_ms = now_ms;
}

static inline float integrator_step_ms(Integrator *i, float rate, uint32_t now_ms) {
    uint32_t dt_ms = now_ms - i->last_ms;  // compute dt first
    i->last_ms = now_ms;                   // then update timestamp

    float dt_s = (float)dt_ms * 0.001f;    // convert ms -> s 
    i->x += rate * dt_s;                   // summation
    return i->x;
}