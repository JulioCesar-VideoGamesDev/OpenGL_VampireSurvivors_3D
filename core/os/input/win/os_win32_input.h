#pragma once
#include "os_input.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


class Win32_Input final : public IInput
{
public:
    using Process_Events_Signature = LRESULT(*)(HWND, UINT, WPARAM, LPARAM);
    static auto CALLBACK process_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT;

    Win32_Input(const Input_Desc& ds = {});
    ~Win32_Input();

    Win32_Input(const Win32_Input&) = delete;
    Win32_Input(Win32_Input&&) = delete;

    auto operator=(const Win32_Input&) -> Win32_Input& = delete;
    auto operator=(Win32_Input&&) -> Win32_Input& = delete;

    auto poll_events() const -> void override;
    auto events_this_frame() const -> const Array<Input_Event>& override;
    auto key_down(u32 key_code) const -> bool override;
    auto set_cursor_mode(Cursor_Mode mode) -> void override;
};
