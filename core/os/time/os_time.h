#pragma once

fn os_reset_time() -> void;
fn os_time_step() -> void;
fn os_delta_time() -> f32;
fn os_fixed_tick_rate() -> u32;
fn os_fps() -> f32;
fn os_av_fps() -> f32;
fn os_set_time_scale(f32 scale) -> void;