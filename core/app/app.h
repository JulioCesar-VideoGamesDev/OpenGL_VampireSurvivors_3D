
#pragma once
#include "os_time.h"
#include "os_window.h"
#include "os_input.h"
#include "os_core.h"

#ifdef GAME_GL
# include "os_gl.h"
#endif

#include "io_audio.h"
#include "io_image.h"
#include "io_model.h"

#include "imgui_state.h"

// @Note: This file is just a C-style easy wrapper for the common stuff. You don't need to use it.

struct App_Desc
{
    std::string working_dir = ".\\assets";
    Window_Desc window;
    Input_Desc input;

#ifdef GAME_DEBUG
    bool init_imgui = true;
#else 
    bool init_imgui = false;
#endif
};

// *** Setup ***

// @Note: Just creates IWindow and IInput instances.
fn app_init(App_Desc ds = {}) -> bool;
fn app_done() -> void;
fn app_running() -> bool;