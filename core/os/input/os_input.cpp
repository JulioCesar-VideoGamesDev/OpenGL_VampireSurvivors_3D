#include "os_input.h"

#if defined (GAME_WIN)
    #define WIN32_MEAN_AND_LEAN
    #include <Windows.h>
    #include "os_win32_input.h"
#endif

auto IInput::create(const Input_Desc& ds) -> Ptr
{
#if defined(GAME_WIN)
    return new Win32_Input(ds);
#else
    return nullptr;
#endif
}

static IInput::Ptr g_input;

fn os_input_init(const Input_Desc& ds) -> bool {
    if (g_input) {
        os_input_done();
    }
    g_input = IInput::create(ds);
    return true;
}

fn os_input_done() -> void {
    delete g_input;
}

fn os_events_this_frame() -> const Array<Input_Event>& {
    return g_input->events_this_frame();
}

fn os_key_down(u32 key_code) -> bool {
    return g_input->key_down(key_code);
}

fn os_set_cursor_mode(Cursor_Mode mode) -> void {
    g_input->set_cursor_mode(mode);
}

fn os_poll_events() -> void {
    return g_input->poll_events();
}
