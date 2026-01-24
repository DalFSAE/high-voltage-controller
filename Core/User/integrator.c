#include "integrator.h"

void integrator_init(Integrator *i, float x0, uint32_t now_ms)
{
    i->x = x0;
    i->last_ms = now_ms;
    i->initialized = true;
}

float integrator_step_ms(Integrator *i, float dx_dt, uint32_t now_ms)
{
    if (!i->initialized) {
        integrator_init(i, 0.0f, now_ms);
        return i->x;
    }

    uint32_t dt_ms = now_ms - i->last_ms;  // compute dt first (wrap-safe)
    i->last_ms = now_ms;                   // then update timestamp

    float dt_s = (float)dt_ms * 0.001f;    // ms -> s
    i->x += dx_dt * dt_s;                  // summation
    return i->x;
}

void differentiator_init(Differentiator *d, float x0, uint32_t now_ms)
{
    d->prev_x = x0;
    d->last_ms = now_ms;
    d->initialized = true;
}

float differentiator_step_ms(Differentiator *d, float x, uint32_t now_ms)
{
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

float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

void pid_init(PID *p,
              float kp, float ki, float kd,
              float out_min, float out_max,
              float i_min, float i_max,
              uint32_t now_ms)
{
    p->kp = kp;
    p->ki = ki;
    p->kd = kd;
    p->out_min = out_min;
    p->out_max = out_max;
    p->i_min = i_min;
    p->i_max = i_max;

    integrator_init(&p->i, 0.0f, now_ms);
    differentiator_init(&p->d, 0.0f, now_ms);
    p->initialized = true;
}

float pid_step_ms(PID *p, float setpoint, float measurement, uint32_t now_ms)
{
    float error = setpoint - measurement;

    // proportional term
    float u_p = p->kp * error;

    // integral term: ui(t) = ki * ∫ e(t) dt
    float error_int = integrator_step_ms(&p->i, error, now_ms);
    error_int = clampf(error_int, p->i_min, p->i_max);
    p->i.x = error_int; // store for next pass
    float u_i = p->ki * error_int;

    // derivative term: ud(t) = kd * d(e)/dt; implemented as -kd * d(meas)/dt
    float dmeas_dt = differentiator_step_ms(&p->d, measurement, now_ms);
    float u_d = -p->kd * dmeas_dt;

    // control signal
    float u = u_p + u_i + u_d;

    return clampf(u, p->out_min, p->out_max);
}
