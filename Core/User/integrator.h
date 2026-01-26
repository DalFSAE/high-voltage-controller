#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

// Integrator
void  integrator_init(Integrator *i, float x0, uint32_t now_ms);
float integrator_step_ms(Integrator *i, float dx_dt, uint32_t now_ms);

// Differentiator
void  differentiator_init(Differentiator *d, float x0, uint32_t now_ms);
float differentiator_step_ms(Differentiator *d, float x, uint32_t now_ms);

// Utility
float clampf(float x, float lo, float hi);

// PID Object
typedef struct  {
    float kp, ki, kd;               // gain terms
    float out_min, out_max;         // output clamp
    float i_min, i_max;             // integrator clamp

    Integrator i;
    Differentiator d;

    bool initialized;
} PID;

void  pid_init(PID *p,
               float kp, float ki, float kd,
               float out_min, float out_max,
               float i_min, float i_max,
               uint32_t now_ms);

float pid_step_ms(PID *p, float setpoint, float measurement, uint32_t now_ms);

#ifdef __cplusplus
}
#endif
