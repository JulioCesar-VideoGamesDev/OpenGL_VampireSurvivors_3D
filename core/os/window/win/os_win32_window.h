#pragma once

#ifdef GAME_WIN

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef GAME_GL
#include "os_win32_gl_context.h"
#endif

#include "os_window.h"

class Win32_Window final : public IWindow
{
public:
    Win32_Window(const Window_Desc& ds = {});
    ~Win32_Window();

    Win32_Window(const Win32_Window&) = delete;
    Win32_Window(Win32_Window&&) = delete;
    
    auto operator=(const Win32_Window&) -> Win32_Window & = delete;
    auto operator=(Win32_Window&&) -> Win32_Window & = delete;
    
    auto handle() const -> void* override;
    auto show() const -> void override;
    auto present(bool vsync) const -> void override;
    
private:
    HWND m_handle = nullptr;
    
    #ifdef GAME_GL
    Win32_GL_Context* m_gl_context = nullptr;
    #endif
};

#endif // GAME_WIN 