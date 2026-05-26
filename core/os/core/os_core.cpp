#include "os_core.h"

#ifdef GAME_WIN
#   include "os_core_win32.h"
#else
#   error "Unsupported OS!"
#endif

#ifdef GAME_WIN

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <direct.h>
#define PATH_SEPARATOR '\\'
#else
#include <limits.h>
#include <unistd.h>    // write, close.
#define PATH_SEPARATOR '/'
#endif

fn get_absolute_path(std::string_view filename) -> std::string {
#ifdef GAME_WIN
    char buffer[MAX_PATH];
    if (GetFullPathNameA(filename.data(), MAX_PATH, buffer, nullptr) == 0) {
        return std::string(filename);  // Fallback to original
    }
    return std::string(buffer);
#else
    char buffer[PATH_MAX];
    if (realpath(filename.data(), buffer) == nullptr) {
        return std::string(filename);  // Fallback to original
    }
    return std::string(buffer);
#endif
}

fn get_parent_path(std::string_view path) -> std::string {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string_view::npos) {
        return ".";  // Current directory
    }
    return std::string(path.substr(0, pos));
}

fn get_stem(std::string_view path) -> std::string {
    // First, get just the filename (without directory)
    size_t dir_pos = path.find_last_of("/\\");
    std::string_view filename = (dir_pos == std::string_view::npos) 
        ? path 
        : path.substr(dir_pos + 1);
    
    // Then remove extension
    size_t ext_pos = filename.find_last_of('.');
    if (ext_pos == std::string_view::npos) {
        return std::string(filename);
    }
    
    return std::string(filename.substr(0, ext_pos));
}

fn get_filename(std::string_view path) -> std::string {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string_view::npos) {
        return std::string(path);
    }
    return std::string(path.substr(pos + 1));
}

fn get_extension(std::string_view path) -> std::string {
    size_t pos = path.find_last_of('.');
    if (pos == std::string_view::npos) {
        return "";
    }
    return std::string(path.substr(pos));
}

fn get_path_info(std::string_view filename) -> Path_Info {
    Path_Info info;
    
    std::string abs_path = get_absolute_path(filename);
    info.dirpath = get_parent_path(abs_path);
    info.name = get_stem(abs_path);
    
    return info;
}

fn os_read_entire_file(std::string_view filename) -> std::string {
    FILE* file = fopen(filename.data(), "rb");
    if (!file) {
        return {};
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size < 0) {
        fclose(file);
        return {};
    }
    
    // Allocate string with exact size
    std::string buffer;
    buffer.resize(static_cast<size_t>(size));
    
    // Read file directly into string's static buffer
    size_t read_size = fread(&buffer[0], 1, size, file);
    
    fclose(file);
    
    // Adjust size if we read less than expected
    if (read_size != static_cast<size_t>(size)) {
        buffer.resize(read_size);
    }
    
    return buffer;
}

fn os_write_entire_file(std::string_view filename, std::string_view content) -> bool {
    FILE* file = fopen(filename.data(), "wb");
    if (!file) {
        return false;
    }
    
    size_t written = fwrite(content.data(), 1, content.size(), file);
    fclose(file);
    
    return written == content.size();
}

fn os_trim(std::string text) -> std::string {
    std::string s = text;
    // left Global::trim
    s.erase(s.begin(),
        std::find_if(s.begin(), s.end(),
            [](unsigned char ch) { return !std::isspace(ch); }));

    // right Global::trim
    s.erase(
        std::find_if(s.rbegin(), s.rend(),
            [](unsigned char ch) { return !std::isspace(ch); }).base(),
        s.end());
    return s;
}

fn os_set_working_dir(const std::string& path) -> void {
#ifdef GAME_WIN
    os_set_working_dir_win32(path);
#else
#   error "Unsupported OS!"
#endif
}

fn os_get_time() -> f64 {
#ifdef GAME_WIN
    return os_get_time_win32();
#else
#   error "Unsupported OS!"
#endif
}