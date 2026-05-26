#ifdef GAME_WIN

#include "os_win32_window.h"
#include "os_win32_input.h"

#define WINDOWED_STYLE           WS_OVERLAPPEDWINDOW
#define FULLSCREEN_STYLE         WS_VISIBLE | WS_POPUP
#define SECONDARY_WINDOW_STYLE   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME

static  auto g_window_class_name = L"Window Class";
static  Win32_Window* g_main_window = nullptr;

Win32_Window::Win32_Window(const Window_Desc& ds)
{
    if (ds.flags & Window_Desc::Flags::MAIN_WINDOW)
    {
        if (ensuref(!g_main_window, "There is already a main window!"))
        {
            g_main_window = this;

            SetProcessDPIAware();
            //SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

            // @Note: This is the handle of the calling process.
            // @Robustness: This does not work on dlls.
            HINSTANCE hInstance = GetModuleHandle(0);

            WNDCLASS wc = { };

            wc.lpfnWndProc = &Win32_Input::process_events;
            // @Note Use this to run the default event processing.
            //wc.lpfnWndProc = &DefWindowProc;

            wc.hInstance = hInstance;
            wc.lpszClassName = g_window_class_name;
            //@Note: CreateSolidBrush changes the background color, I guess.
            wc.hbrBackground = CreateSolidBrush(RGB(ds.bg_color.x * 255, ds.bg_color.y * 255, ds.bg_color.z * 255));

            if (!ensuref(RegisterClass(&wc) != 0, "Couldn't register the Win32 window class.\n")) return;

            logf("Win32 Window class registered!");
        }
    }
    else
    {
        if (!ensuref(g_main_window, "We need a Main Window created first!"))
        {
            return;
        }
    }

    RECT rect;
    rect.top = 0;
    rect.left = 0;
    rect.right = ds.width;
    rect.bottom = ds.height;

    constexpr auto s_style = WINDOWED_STYLE;

    AdjustWindowRect(&rect, s_style, false);

    auto client_width = rect.right - rect.left;
    auto client_height = rect.bottom - rect.top;

    HWND hwnd = CreateWindowEx(
        0,                          // Optional window styles.
        g_window_class_name,        // Window class.
        ds.title.data(),            // Window title
        s_style,                    // Window style.
        CW_USEDEFAULT,              // X Pos.
        CW_USEDEFAULT,              // Y Pos.
        client_width,               // Width.
        client_height,              // Height.
        nullptr,                    // Parent window.
        nullptr,                    // Menu.
        static_cast<HINSTANCE>(0),  // Instance handle. @Robustness: Apparently only required for Win98.
        nullptr                     // Additional application data.
    );

    if (ensuref(hwnd != 0, "Couldn't create the Win32 window!"))
    {
        m_handle = hwnd;
        if (ds.flags & Window_Desc::Flags::SHOW) show();
    }

    logf("Win32 Window created!");

#if defined(GAME_GL)
    if (ds.flags & Window_Desc::Flags::CONTEXT)
    {
        m_gl_context = new Win32_GL_Context(m_handle);
    }
#endif
    
    if (ds.flags & Window_Desc::Flags::MAIN_WINDOW)
    {
        // @Note: Register the input device.
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01; // Generic Desktop
        rid.usUsage = 0x02; // Mouse
        rid.dwFlags = RIDEV_INPUTSINK;
        rid.hwndTarget = hwnd;
        RegisterRawInputDevices(&rid, 1, sizeof(rid));
    }

    if (hwnd) {
        SetForegroundWindow(hwnd);
    }
}

Win32_Window::~Win32_Window()
{
    if (m_handle)
    {
        if (this == g_main_window)
        {
            // @Note: Unregister the input device.
            RAWINPUTDEVICE rid;
            rid.usUsagePage = 0x01; // Generic Desktop
            rid.usUsage = 0x02; // Mouse
            rid.dwFlags = RIDEV_REMOVE;
            rid.hwndTarget = m_handle;
            RegisterRawInputDevices(&rid, 1, sizeof(rid));
        }

        DestroyWindow(m_handle);
        m_handle = nullptr;
        logf("Win32 Window destroyed!");
    }
    if (m_gl_context)
    {
        delete m_gl_context;
        m_gl_context = nullptr;
    }
}

auto Win32_Window::show() const -> void
{
    UpdateWindow(m_handle);
    ShowWindow(m_handle, SW_SHOW);
}

auto Win32_Window::handle() const -> void*
{
    return m_handle;
}

auto Win32_Window::present(bool vsync) const -> void
{
#if defined(GAME_GL)
    if (ensuref(m_gl_context, "We need a wgl context to present! \n"))
    {
        m_gl_context->swap_interval(vsync ? 1 : 0);
        m_gl_context->swap_buffers();
    }
#elif defined(GAME_D3D11)
    renderer().present(vsync);
#endif
}

#endif // GAME_WIN