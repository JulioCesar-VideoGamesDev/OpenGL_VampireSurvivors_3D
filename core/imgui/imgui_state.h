#pragma once
#include "imgui.h"

fn imgui_init(const class IWindow& window) -> void;
fn imgui_done() -> void;
fn imgui_frame_init() -> void;
fn imgui_frame_done() -> void;
fn imgui_is_init() -> bool;
fn imgui_draw_text(Vec2 pos, f32 font_size, Vec4 color, const char* fmt, ...) -> void;

using Draw_ImGui_Fn = void(*)();

fn imgui_draw_frame(Draw_ImGui_Fn draw) -> void;

// @Hack: We have to do this before ImGui::DestroyContext.
// Why? Because when we reach DestoyContext ImGui have already zeroed the window positions.
// Why? No fucking clue.
fn imgui_force_save_ini() -> void;