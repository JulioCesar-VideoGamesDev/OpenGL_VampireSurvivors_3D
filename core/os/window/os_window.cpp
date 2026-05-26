#include "os_window.h"

#if defined (GAME_WIN)
    #define WIN32_MEAN_AND_LEAN
    #include <Windows.h>
    #include "os_win32_window.h"
#endif

auto IWindow::create(const Window_Desc& ds) -> Ptr
{
#if defined(GAME_WIN)
    return new Win32_Window(ds);
#else
    return nullptr;
#endif
}


IWindow::Ptr g_window;

fn os_window_init(Window_Desc ds) -> bool {   
    if (g_window) {
        os_window_done();
    }
    g_window = IWindow::create(ds);
    return true;
}

fn os_window_done() -> void {
    delete g_window;
}

fn os_window() -> IWindow& {
    return *g_window;
}

fn os_swap_buffers(bool vsync) -> void {
    g_window->present(vsync);
}