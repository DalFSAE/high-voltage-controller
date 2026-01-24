#pragma once

// Mock Tractive System
// Goal: Model that can be used for unit testing

#include <math.h>
#include <stdint.h>


static inline float rc_step_vc(float v_prev, float v_in, float r_ohm, float c_f, uint32_t delta_t_ms) {
    float tau = r_ohm * c_f;
    float dt_s = delta_t_ms / 1000.0f;
    
    return v_prev + (dt_s / tau) * (v_in - v_prev);
}