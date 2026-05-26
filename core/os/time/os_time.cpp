#include "os_time.h"
#include "os_core.h"

static constexpr u32 FPS_SAMPLES = 30u;

struct Time {
    f64 seconds = -1.f;
    u32 frames = 0u;
    f32 dt = 0.f;
    f64 dt_max = 0.06;
    f64 dt_fixed = 0.06;
    f64 dt_fixed_acc = 0.f;
    f64 last_time = 0.f;
    u32 fixed_tick_rate = 0u;
    f32 scale = 1.f;

    f32 dts[FPS_SAMPLES] = {};
    u32 frame_index = 0u;
    f32 frame_total = 0.f;
} g_time;

fn os_reset_time() -> void {
    g_time = {};
}

fn os_time_step() -> void {
    auto& time = g_time;
    
    //@Note: Lazy init.
    if (time.seconds < 0) {
        time.last_time = os_get_time();
        time.dt_fixed_acc = 0.f;
        time.seconds = 0;
    }

    f64 current_time = os_get_time();
    time.frames += 1;
    f64 time_between_frames = current_time - time.last_time;
    time.last_time = current_time;
    time.seconds += time_between_frames;
    
    f64 real_dt = clamp(time_between_frames, 0.0, time.dt_max);
    time.dt = (f32)(real_dt * time.scale);

    time.dt_fixed_acc += real_dt;
    
    while (time.dt_fixed_acc >= time.dt_fixed) {
        time.dt_fixed_acc -= time.dt_fixed;
        time.fixed_tick_rate += 1;
    }
    
    time.frame_total -= time.dts[time.frame_index];
    time.dts[time.frame_index] = time.dt;
    time.frame_total += time.dt;
    time.frame_index = (time.frame_index + 1) % FPS_SAMPLES;
}

fn os_fixed_tick_rate() -> u32 {
    return g_time.fixed_tick_rate;
}

fn os_delta_time() -> f32 {
    return g_time.dt;
}

fn os_fps() -> f32 {
    f32 av = g_time.frame_total / FPS_SAMPLES;
    return av > 0.f ? 1.f / av : 0.f;
}

fn os_av_fps() -> f32 {
    return (f32) g_time.frames / (f32) g_time.seconds;
}

fn os_set_time_scale(f32 scale) -> void {
    g_time.scale = scale;
}