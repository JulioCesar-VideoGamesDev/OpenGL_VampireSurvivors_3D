#pragma once

#if GAME_WIN
    #include <Windows.h>
    #include "GL/GL.h"
#else
    #error "Unsupported OS!"
#endif

#include "glext.h"

// @Note: Lets cast some X macro spells...

#define FOR_GL_FUNCTIONS(DO) \
    DO(PFNGLDEBUGMESSAGECALLBACKPROC,      glDebugMessageCallback)      \
    DO(PFNGLCREATESHADERPROC,              glCreateShader)              \
    DO(PFNGLDELETESHADERPROC,              glDeleteShader)              \
    DO(PFNGLSHADERSOURCEPROC,              glShaderSource)              \
    DO(PFNGLCOMPILESHADERPROC,             glCompileShader)             \
    DO(PFNGLGETSHADERIVPROC,               glGetShaderiv)               \
    DO(PFNGLGETSHADERINFOLOGPROC,          glGetShaderInfoLog)          \
    DO(PFNGLCREATEPROGRAMPROC,             glCreateProgram)             \
    DO(PFNGLDELETEPROGRAMPROC,             glDeleteProgram)             \
    DO(PFNGLATTACHSHADERPROC,              glAttachShader)              \
    DO(PFNGLLINKPROGRAMPROC,               glLinkProgram)               \
    DO(PFNGLGETPROGRAMIVPROC,              glGetProgramiv)              \
    DO(PFNGLGETPROGRAMINFOLOGPROC,         glGetProgramInfoLog)         \
    DO(PFNGLUSEPROGRAMPROC,                glUseProgram)                \
    DO(PFNGLGETUNIFORMLOCATIONPROC,        glGetUniformLocation)        \
    DO(PFNGLUNIFORM1IVPROC,                glUniform1iv)                \
    DO(PFNGLCREATEBUFFERSPROC,             glCreateBuffers)             \
    DO(PFNGLDELETEBUFFERSPROC,             glDeleteBuffers)             \
    DO(PFNGLNAMEDBUFFERDATAPROC,           glNamedBufferData)           \
    DO(PFNGLNAMEDBUFFERSUBDATAPROC,        glNamedBufferSubData)        \
    DO(PFNGLCREATEVERTEXARRAYSPROC,        glCreateVertexArrays)        \
    DO(PFNGLDELETEVERTEXARRAYSPROC,        glDeleteVertexArrays)        \
    DO(PFNGLVERTEXARRAYELEMENTBUFFERPROC,  glVertexArrayElementBuffer)  \
    DO(PFNGLVERTEXARRAYVERTEXBUFFERPROC,   glVertexArrayVertexBuffer)   \
    DO(PFNGLENABLEVERTEXARRAYATTRIBPROC,   glEnableVertexArrayAttrib)   \
    DO(PFNGLVERTEXARRAYATTRIBFORMATPROC,   glVertexArrayAttribFormat)   \
    DO(PFNGLVERTEXARRAYATTRIBIFORMATPROC,  glVertexArrayAttribIFormat)  \
    DO(PFNGLVERTEXARRAYATTRIBBINDINGPROC,  glVertexArrayAttribBinding)  \
    DO(PFNGLVERTEXARRAYBINDINGDIVISORPROC, glVertexArrayBindingDivisor) \
    DO(PFNGLBINDVERTEXARRAYPROC,           glBindVertexArray)           \
    DO(PFNGLBINDBUFFERBASEPROC,            glBindBufferBase)            \
    DO(PFNGLCREATETEXTURESPROC,            glCreateTextures)            \
    DO(PFNGLTEXTURESTORAGE2DPROC,          glTextureStorage2D)          \
    DO(PFNGLTEXTURESUBIMAGE2DPROC,         glTextureSubImage2D)         \
    DO(PFNGLTEXTUREPARAMETERIPROC,         glTextureParameteri)         \
    DO(PFNGLBINDTEXTUREUNITPROC,           glBindTextureUnit)           \

// @Note: We define GL_PROCS_NO_EXTERN just in one translation unit (gl_context.cpp)
// So that the compiler knows that which is the impl file, and which ones are just declaration files.

#ifndef GL_PROCS_NO_EXTERN
    #define DO_DECLARATIONS(_SIGNATURE, _NAME) \
        extern _SIGNATURE _NAME;
#else
    #define DO_DECLARATIONS(_SIGNATURE, _NAME) \
        _SIGNATURE _NAME = nullptr; 
#endif

FOR_GL_FUNCTIONS(DO_DECLARATIONS)

// @Note: Helper functions

inline fn os_compile_gl_shader_with_prefix(std::string_view source, std::string_view prefix, GLenum type) -> GLuint {
    GLuint shader = glCreateShader(type);

    const char* sources[2];
    s32 lenghts[2];

    sources[0] = prefix.data();
    lenghts[0] = static_cast<s32>(prefix.length());
    
    sources[1] = source.data();
    lenghts[1] = static_cast<s32>(source.length());

    glShaderSource(shader, 2, sources, lenghts);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        // @Pending: Wrap this stuff into a macro.
        static  constexpr GLsizei LOG_BUFFER_SIZE = 512;
        GLchar log_buffer[LOG_BUFFER_SIZE];
        GLsizei log_lenght;
        glGetShaderInfoLog(shader, LOG_BUFFER_SIZE, &log_lenght, log_buffer);
        logf("Error compiling the program.\n%s", std::string_view(log_buffer, log_lenght).data());
        return 0u;
    }

    return shader;
}

inline fn os_create_gl_program(std::string_view source) -> GLuint {
    if (source.empty())
    {
        logf("Error! The shader source cannot be empty!");
        return 0u;
    }
    
    static  constexpr const char* vert_prefix =
        "#version 460 core \n"
        "#define VERTEX_SHADER \n";
    
    GLuint vert = os_compile_gl_shader_with_prefix(source, vert_prefix, GL_VERTEX_SHADER);

    if (vert == 0u)
    {
        logf("Error compiling the vertex shader.");
        return 0u;
    }

    static  constexpr const char* frag_prefix =
        "#version 460 core \n"
        "#define FRAGMENT_SHADER \n";

    GLuint frag = os_compile_gl_shader_with_prefix(source, frag_prefix, GL_FRAGMENT_SHADER);

    if (frag == 0u)
    {
        logf("Error compiling the fragment shader.");
        return 0u;
    }
    
    // @Note: From here we create the program and link the shaders.

    GLuint prog = glCreateProgram();

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glLinkProgram(prog);

    glDeleteShader(vert);
    glDeleteShader(frag);
    
    GLint success;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        static  constexpr GLsizei LOG_BUFFER_SIZE = 512;
        GLchar log_buffer[LOG_BUFFER_SIZE];
        GLsizei log_lenght;
        glGetProgramInfoLog(prog, LOG_BUFFER_SIZE, &log_lenght, log_buffer);
        logf("Error linking the program.\n%s", std::string_view(log_buffer, log_lenght).data());
        return 0u;
    }
    
    return prog;
}

inline fn os_clear_color_gl(const Vec4& color) -> void {
    glClearColor(color.x, color.y, color.z, color.w);
}

inline fn os_is_gl_attribute(Data_Type type) -> bool {
    switch(type) {
        case Data_Type::Float:    
        case Data_Type::Float2:   
        case Data_Type::Float3:
        case Data_Type::Float4:
        case Data_Type::Mat3:
        case Data_Type::Mat4:
        case Data_Type::Int:       
        case Data_Type::Int2:    
        case Data_Type::Int3:  
        case Data_Type::Int4: 
            return true;
        case Data_Type::Sampler2D:
        case Data_Type::Bool:
        case Data_Type::None:
            return false;
    }
    return false;
}

inline fn os_from_gl(GLint type) -> Data_Type {
    switch(type) {
        case GL_FLOAT      : return Data_Type::Float;
        case GL_FLOAT_VEC2 : return Data_Type::Float2;
        case GL_FLOAT_VEC3 : return Data_Type::Float3;
        case GL_FLOAT_VEC4 : return Data_Type::Float4;
        case GL_FLOAT_MAT3 : return Data_Type::Mat3;
        case GL_FLOAT_MAT4 : return Data_Type::Mat4;
        case GL_INT        : return Data_Type::Int;
        case GL_INT_VEC2   : return Data_Type::Int2;
        case GL_INT_VEC3   : return Data_Type::Int3;
        case GL_INT_VEC4   : return Data_Type::Int4;
        case GL_SAMPLER_2D : return Data_Type::Sampler2D;
        case GL_BOOL       : return Data_Type::Bool;
    }
    return Data_Type::None;
}

inline fn os_to_gl(Data_Type type) -> GLint {
    switch(type) {
        case Data_Type::Float     : return GL_FLOAT;
        case Data_Type::Float2    : return GL_FLOAT;
        case Data_Type::Float3    : return GL_FLOAT;
        case Data_Type::Float4    : return GL_FLOAT;
        case Data_Type::Mat3      : return GL_FLOAT;
        case Data_Type::Mat4      : return GL_FLOAT;
        case Data_Type::Int       : return GL_INT;
        case Data_Type::Int2      : return GL_INT;
        case Data_Type::Int3      : return GL_INT;
        case Data_Type::Int4      : return GL_INT;
        case Data_Type::Sampler2D : return GL_SAMPLER_2D;
        case Data_Type::Bool      : return GL_BOOL;
        case Data_Type::None      : return 0;
    }
    return 0;
};