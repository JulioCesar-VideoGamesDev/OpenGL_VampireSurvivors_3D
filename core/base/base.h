#pragma once

#define fn auto

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef signed char        s8;
typedef short              s16;
typedef int                s32;
typedef long long          s64;

typedef float    f32;
typedef double   f64;

enum class Data_Type : u8
{
    None,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Sampler2D,
    Bool
};

inline fn is_integer_type(Data_Type type) -> bool {
    switch(type) {
        case Data_Type::Float:    
        case Data_Type::Float2:   
        case Data_Type::Float3:
        case Data_Type::Float4:
        case Data_Type::Mat3:
        case Data_Type::Mat4:
            return false;
        case Data_Type::Int:       
        case Data_Type::Sampler2D:
        case Data_Type::Int2:    
        case Data_Type::Int3:  
        case Data_Type::Int4: 
        case Data_Type::Bool:
        case Data_Type::None:
            return true;
    }
    return false;
}

// @Note: Returns the size in bytes.
inline fn get_size(Data_Type type) -> u32 {
    switch(type) {
        case Data_Type::Float:      return 4;
        case Data_Type::Float2    : return 4 * 2;
        case Data_Type::Float3    : return 4 * 3;
        case Data_Type::Float4    : return 4 * 4;
        case Data_Type::Mat3      : return 4 * 3 * 3;
        case Data_Type::Mat4      : return 4 * 4 * 4;
        case Data_Type::Int       : return 4;
        case Data_Type::Sampler2D : return 32;
        case Data_Type::Int2      : return 4 * 2;
        case Data_Type::Int3      : return 4 * 3;
        case Data_Type::Int4      : return 4 * 4;
        case Data_Type::Bool      : return 1;
        case Data_Type::None      : return 0;
    }
    return 0;
}

// @Note: Returns the number of elements.
inline fn get_count(Data_Type type) -> u32 {
    switch(type) {
        case Data_Type::Float     : return 1;         
        case Data_Type::Float2    : return 2;
        case Data_Type::Float3    : return 3;
        case Data_Type::Float4    : return 4;
        case Data_Type::Mat3      : return 3 * 3;
        case Data_Type::Mat4      : return 4 * 4;
        case Data_Type::Int       : return 1;
        case Data_Type::Sampler2D : return 32;
        case Data_Type::Int2      : return 2;
        case Data_Type::Int3      : return 3;
        case Data_Type::Int4      : return 4;
        case Data_Type::Bool      : return 1;
        case Data_Type::None      : return 0;
    }
    return 0;
}

// @Note: Simple log macro implementation.

#ifdef GAME_DEBUG
    fn _logf_function(const char* fmt, ...) -> void;
#endif

#ifdef GAME_DEBUG
#   define logf(X, ...) (_logf_function(X, __VA_ARGS__))
#else
#   define logf(X, ...)
#endif

// @Note: Debug break macro implementation.

#ifdef GAME_DEBUG
#   ifdef GAME_WIN
#       define dbg_break() __debugbreak()
#   else
#       error "Unsupported OS!"
#   endif
#else
#   define dbg_break()
#endif

// @Note: Assert macro implementation.

#ifdef GAME_DEBUG
fn _checkf_function(bool expr, const char* fmt, ...) -> void;
fn _ensuref_function(bool expr, const char* fmt, ...) -> bool;
#endif

#ifdef GAME_DEBUG
#   define checkf(X, ...) (_checkf_function(X, __VA_ARGS__))
#   define ensuref(X, ...) (_ensuref_function(X, __VA_ARGS__))
#else
#   define checkf(...)
#   define ensuref(X, ...) (X)
#endif

#define Stringify(_x) #_x