#include "os_win32_input.h"
#include "imgui_impl_win32.h"

static auto g_input_created = false;
static Array<Input_Event> g_events;
static Key_State::Type g_key_states[Key_Code::Count] = {};
static bool g_key_down_table[Key_Code::Count] = {};
static Cursor_Mode g_cursor_mode = Cursor_Mode::Default;

static fn from_windows_format(WPARAM wParam) -> Key_Code::Type {
    // *** ASCII KEYS ***
    if (wParam >= '0' && wParam <= '9')
    {
        return (Key_Code::Type) wParam;
    }

    if (wParam >= 'A' && wParam <= 'Z')
    {
        return (Key_Code::Type) wParam;
    }

    // *** NON ASCII ***
    switch (wParam)
    {
    case VK_BACK:       return Key_Code::Backspace;
    case VK_TAB:        return Key_Code::Tab;
    case VK_RETURN:     return Key_Code::Enter;
    case VK_ESCAPE:     return Key_Code::Escape;
    case VK_SPACE:      return Key_Code::Space;

    case VK_DELETE:     return Key_Code::Delete;
    case VK_UP:         return Key_Code::Arrow_Up;
    case VK_DOWN:       return Key_Code::Arrow_Down;
    case VK_LEFT:       return Key_Code::Arrow_Left;
    case VK_RIGHT:      return Key_Code::Arrow_Right;
    case VK_PRIOR:      return Key_Code::Page_Up;
    case VK_NEXT:       return Key_Code::Page_Down;
    case VK_HOME:       return Key_Code::Home;
    case VK_END:        return Key_Code::End;
    case VK_INSERT:     return Key_Code::Insert;
    case VK_PAUSE:      return Key_Code::Pause;
    case VK_SCROLL:     return Key_Code::Scroll_Lock;

    case VK_MENU:       return Key_Code::Alt;
    case VK_CONTROL:    return Key_Code::Ctrl;
    case VK_SHIFT:      return Key_Code::Shift;
    case VK_LWIN:
    case VK_RWIN:       return Key_Code::CMD;

    case VK_SNAPSHOT:   return Key_Code::Print_Screen;

    // @Note: Apparently this doesn't work.
    //case VK_LBUTTON:    return Key_Code::Mouse_Left;
    //case VK_MBUTTON:    return Key_Code::Mouse_Middle;
    //case VK_RBUTTON:    return Key_Code::Mouse_Right;
    }
    
    // *** FUNCTION KEYS ***
    if (wParam >= VK_F1 && wParam <= VK_F24)
    {
        return (Key_Code::Type) (Key_Code::F1 + (wParam - VK_F1));
    }

    return Key_Code::Unknown;
}

static fn update_cursor_state() -> void {
    if (g_cursor_mode == Cursor_Mode::Default)
    return;

    HWND fg = GetForegroundWindow();
    
    RECT rect;
    GetClientRect(fg, &rect);
    
    POINT ul = { rect.left,  rect.top };
    POINT lr = { rect.right, rect.bottom };

    ClientToScreen(fg, & ul);
    ClientToScreen(fg, &lr);

    rect.left = ul.x;
    rect.top = ul.y;
    rect.right = lr.x;
    rect.bottom = lr.y;
    
    ClipCursor(&rect);
    
    if (g_cursor_mode == Cursor_Mode::Hidden)
    {
        // @Note: We center the cursor.
        SetCursorPos(
            (rect.left + rect.right) / 2,
            (rect.top + rect.bottom) / 2
        );
    }
}

// @Note Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

fn CALLBACK Win32_Input::process_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

    if (!g_input_created) {
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_INPUT: {
            UINT size = 0u;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

            BYTE buffer[64];
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));
            
            RAWINPUT* raw = (RAWINPUT*)buffer;
            
            if (raw->header.dwType == RIM_TYPEMOUSE) {
                LONG dx = raw->data.mouse.lLastX;
                LONG dy = raw->data.mouse.lLastY;
                
                auto& ev = append(&g_events);
                ev.type = Input_Event::Mouse_Move;
                
                ev.mouse_delta_x = dx;
                ev.mouse_delta_y = dy;

                ev.mouse_x = (s32)(short)LOWORD(lParam);
                ev.mouse_y = (s32)(short)HIWORD(lParam);
            }
            break;
        }
        case WM_SIZE: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Window;
            ev.window_x = LOWORD(lParam);
            ev.window_y = HIWORD(lParam);
            update_cursor_state();
        }
        break;

        case WM_MOVE: {
            update_cursor_state();
        }
        break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Key;
            ev.key_code = from_windows_format(wParam);

            ev.key_state = Key_State::Down;

            bool is_repeat = (lParam & (1 << 30)) != 0;

            if (!is_repeat)
                ev.key_state |= Key_State::Start;

            g_key_down_table[ev.key_code] = true;
        }
        break;

        case WM_KEYUP:
        case WM_SYSKEYUP: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Key;
            ev.key_code = from_windows_format(wParam);

            ev.key_state = Key_State::End;

            g_key_down_table[ev.key_code] = false;
        }
        break;

        case WM_MOUSEMOVE: {
            update_cursor_state();
        }
        break;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Key;
            ev.key_code = Key_Code::Mouse_Left;
            using namespace Key_State;
            bool down = msg == WM_LBUTTONDOWN;
            ev.key_state = down ? Down : End;
            g_key_down_table[ev.key_code] = down;
        }
        break;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Key;
            ev.key_code = Key_Code::Mouse_Right;
            using namespace Key_State;
            bool down = msg == WM_RBUTTONDOWN;
            ev.key_state = down ? Down : End;
            g_key_down_table[ev.key_code] = down;
        }
        break;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Key;
            ev.key_code = Key_Code::Mouse_Middle;
            using namespace Key_State;
            bool down = msg == WM_MBUTTONDOWN;
            ev.key_state = down ? Down : End;
            g_key_down_table[ev.key_code] = down;
        }
        break;

        case WM_MOUSEWHEEL: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Mouse_Wheel;
            ev.wheel_delta = (s32)GET_WHEEL_DELTA_WPARAM(wParam);
        }
        break;

        case WM_CLOSE:
        case WM_QUIT: {
            auto& ev = append(&g_events);
            ev.type = Input_Event::Quit;
        }
        break;
        case WM_KILLFOCUS:
            memset(g_key_down_table, 0, sizeof(g_key_down_table));
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// @Pending: Add some state.

Win32_Input::Win32_Input(const Input_Desc& ds)
{
    if (ensuref(!g_input_created, "We don't support multiple instances for the Input class.\n"))
    {
        g_input_created = true;
        logf("Win32 Input created!");
    }
}

Win32_Input::~Win32_Input()
{
    if (g_input_created)
    {
        g_input_created = false;
    }
    logf("Win32 Input destroyed!");
}

auto Win32_Input::poll_events() const -> void
{
    reset_keeping_memory(&g_events);

    auto msg = MSG{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

auto Win32_Input::events_this_frame() const -> const Array<Input_Event>&
{
    return g_events;
}

auto Win32_Input::key_down(u32 key_code) const -> bool
{
    return g_key_down_table[key_code];
}

auto Win32_Input::set_cursor_mode(Cursor_Mode mode) -> void
{    
    if (mode == g_cursor_mode)
        return;

    if (mode == Cursor_Mode::Default)
    {
        ClipCursor(NULL);

        // @Robustness: This won't work on multiple windows in different threads.
        while (ShowCursor(TRUE) < 0) {}
    }
    else
    {
        // @Robustness: This won't work on multiple windows in different threads.
        while (ShowCursor(FALSE) >= 0) {}
    }

    g_cursor_mode = mode;
}
