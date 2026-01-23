#pragma once

#include <math.h>
#include <stdint.h>

static inline float rc_step_vc(float vc_prev, float vin, float R_ohm, float C_f, uint32_t delta_t_ms)
{
    if (R_ohm <= 0.0f || C_f <= 0.0f) {
        // Degenerate: no filtering (or invalid params) -> follow input
        return vin;
    }

    const float dt_s = (float)delta_t_ms * 1e-3f;
    const float tau  = R_ohm * C_f;

    // If dt is huge, exp(-dt/tau) underflows toward 0, which is fine.
    const float a = expf(-dt_s / tau);

    return vin + (vc_prev - vin) * a;
}