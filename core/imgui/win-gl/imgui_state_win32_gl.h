#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

fn imgui_init_win32_gl(HWND hwnd, HDC hdc, HGLRC hglrc) -> void;
fn imgui_done_win32_gl() -> void;
fn imgui_frame_init_win32_gl() -> void;
fn imgui_frame_done_win32_gl() -> void;