#pragma once

struct Window_Desc
{
    enum Flags : u8
    {
        NONE = 0,
        SHOW = 1 << 0,
        CONTEXT = 1 << 1,
        MAIN_WINDOW = 1 << 2,
        ALL = SHOW | CONTEXT | MAIN_WINDOW,
    };
    
    std::wstring_view title = L"The Window";
    s32 width  = 1280;
    s32 height = 720;
    Flags flags = ALL;
    Vec3 bg_color = Color.Chill_Green;
};

fn os_window_init(Window_Desc ds) -> bool;
fn os_window_done() -> void;
fn os_window() -> class IWindow&;
fn os_swap_buffers(bool vsync = true) -> void;

class IWindow
{
public:
    using Ptr = IWindow*;

    static auto create(const Window_Desc& ds = {})->Ptr;

    virtual ~IWindow() = default;
    
    virtual auto handle() const -> void* = 0;
    virtual auto show() const -> void = 0;
    virtual auto present(bool vsync = true) const -> void = 0;
};
