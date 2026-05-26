#pragma once

using Audio_Handle = Array_Handle;

// @Note: Don't worry about this two.
// ------- 
// The app does the the audio system initialization.
fn io_audio_init() -> bool;
fn io_audio_done() -> void;
// -------

fn io_audio_load(const std::string& filename) -> Audio_Handle;
fn io_audio_free(const Audio_Handle& handle) -> void;
fn io_audio_play(const Audio_Handle& handle) -> void;
fn io_audio_stop(const Audio_Handle& handle) -> void;
fn io_audio_set_volume(const Audio_Handle& handle, f32 volume) -> void;