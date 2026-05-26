#include "os_core_win32.h"

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

// @Pending: Use this in the window title.
// Convert UTF-8 string to wide string (Windows UTF-16)
fn utf8_to_wide(std::string_view utf8) -> std::wstring {
    if (utf8.empty()) {
        return {};
    }
    
    // Get required buffer size
    int size_needed = MultiByteToWideChar(
        CP_UTF8, 0,
        utf8.data(), (int)utf8.size(),
        nullptr, 0
    );
    
    if (size_needed <= 0) {
        return {};
    }
    
    // Allocate and convert
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8, 0,
        utf8.data(), (int)utf8.size(),
        &result[0], size_needed
    );
    
    return result;
}

fn os_set_working_dir_win32(std::string_view path) -> void {
    if (path.empty()) {
        return;
    }
    
    // Convert UTF-8 to wide string properly
    std::wstring working_dir = utf8_to_wide(path);
    SetCurrentDirectoryW(working_dir.c_str());
}

auto os_get_time_win32() -> f64
{
    static LARGE_INTEGER frequency;
    static BOOL initialized = FALSE;

    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        initialized = TRUE;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    // @Note: Ticks divided by ticks-per-second.
    return (f64) counter.QuadPart / (f64) frequency.QuadPart;
}