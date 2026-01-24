#pragma once

#include <stdbool.h>
#include <stdint.h> 

typedef struct {
    float x;
    uint32_t last_ms;
    bool initialized;
} Integrator;

typedef struct {
    float prev_x;
    uint32_t last_ms;
    bool initialized;
} Differentiator;

static inline void integrator_init(Integrator *i, float x0, uint32_t now_ms)
{
    i->x = x0;
    i->last_ms = now_ms;
    i->initialized = true;
}

static inline float integrator_step_ms(Integrator *i, float dx_dt, uint32_t now_ms) {
    if (!i->initialized) {
        integrator_init(i, 0.0f, now_ms);
        return i->x;
    }
    
    uint32_t dt_ms = now_ms - i->last_ms;  // compute dt first
    i->last_ms = now_ms;                   // then update timestamp

    float dt_s = (float)dt_ms * 0.001f;    // convert ms -> s 
    i->x += dx_dt * dt_s;                   // summation
    return i->x;
}


static inline void differentiator_init(Differentiator *d, float x0, uint32_t now_ms) {
    d->prev_x = x0;
    d->last_ms = now_ms;
    d->initialized = true;
}

static inline float differentiator_step_ms(Differentiator *d, float x, uint32_t now_ms) {
    if (!d->initialized) {
        differentiator_init(d, x, now_ms);
        return 0.0f;
    }

    uint32_t dt_ms = now_ms - d->last_ms;
    d->last_ms = now_ms;

    if (dt_ms == 0) {
        d->prev_x = x;
        return 0.0f;
    }

    float dt_s = (float)dt_ms * 0.001f;
    float dx = x - d->prev_x;
    d->prev_x = x;

    return dx / dt_s;
}
