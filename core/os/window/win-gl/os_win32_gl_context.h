#pragma once

#if defined (GAME_WIN) && defined(GAME_GL)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// @Pending: Error handling!
class Win32_GL_Context 
{
public:
    Win32_GL_Context(HWND window_handle);
    ~Win32_GL_Context();

    Win32_GL_Context(const Win32_GL_Context&) = delete;
    Win32_GL_Context(Win32_GL_Context&&) = delete;

    auto operator=(const Win32_GL_Context&) -> Win32_GL_Context& = delete;
    auto operator=(Win32_GL_Context&&) -> Win32_GL_Context& = delete;
    
    auto swap_interval(s32 interval) -> void;
    auto swap_buffers() const -> void;

private:
    HGLRC m_context = nullptr;
    HDC m_device = nullptr;
};

#endif