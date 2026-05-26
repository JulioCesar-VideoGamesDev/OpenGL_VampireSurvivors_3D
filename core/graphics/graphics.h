#pragma once

fn clear_back_buffer(Vec4 color = Color.Black) -> void;
fn set_blend_enabled(bool enabled = true) -> void;
fn set_viewport(s32 viewport_x, s32 viewport_y) -> void;
fn set_depth_test_enabled(bool enabled = true) -> void;

struct Vert_View {
    const void* data = nullptr;
    s32 size = 0;
    s32 count = 0;
};

struct Elem_View {
    const u32* data = nullptr;
    s32 count = 0;
};

struct Attr_View {
    const Data_Type* data = nullptr;
    s32 count = 0;
};

struct Vertex_Buffer_Def {
    Vert_View verts;
    Elem_View elems;
    Attr_View attrs;
};

struct Vertex_Buffer {
    u32 vao = 0u;
    u32 ebo = 0u;
    u32 vbo = 0u;
    s32 elem_count = 0u;
};

fn vertex_buffer_init(Vertex_Buffer* obj, Vertex_Buffer_Def def) -> void;
fn vertex_buffer_done(Vertex_Buffer* obj) -> void;
fn vertex_buffer_draw(Vertex_Buffer obj, u32 count = 0, u32 index_offset = 0) -> void;

struct Shader_Def {
    std::string_view filename;
};

struct Shader {
    u32 pgm = 0u;
};

fn shader_init(Shader* shader, Shader_Def def) -> void;
fn shader_done(Shader* shader) -> void;
fn shader_use(Shader shader) -> void;
fn shader_set_param(Shader shader, std::string_view name, const s32* data, s32 count) -> void;

struct Global_Buffer_Def {
    s32 size = 0;
};

// @Note: The aligned byte offset of a variable must be equal to a multiple of its base alignment.
struct Global_Buffer {
    u32 gbo = 0u;
    s32 size = 0;
};

fn global_buffer_init(Global_Buffer* obj, Global_Buffer_Def def) -> void;
fn global_buffer_done(Global_Buffer* obj) -> void;
fn global_buffer_use(Global_Buffer obj, u32 index = 0) -> void;

fn global_buffer_update(Global_Buffer obj, const void* data) -> void;

enum Texture_Filter: u8 {
    // Convierte u, v a coordenadas de texel, devuelve el texel más cercano.
    Texture_Filter_Nearest = 0,
    // Localiza los cuatro texels vecinos alrededor de las u, v e interpola los
    // colores basándose en la distancia al centro.
    Texture_Filter_Linear,
    // (Con minmaps):
    //Bilinear,
    //Trilinear,
};

enum Texture_Kind: u8 {
    Texture_Kind_Single = 0,
    Texture_Kind_Multiple
};

struct Texture_Def {
    std::string_view filename = "";
    const struct IO_Image* image = nullptr;
    u8 filter = Texture_Filter_Nearest; 
    u8 kind = Texture_Kind_Single;
    s32 subtex_size = 0;  
};

struct Subtex {
    s32 x = 0;
    s32 y = 0;
    s32 width = 0;
    s32 height = 0;
};

struct Subtex_Array {
    static constexpr s32 max = 25;
    Subtex data[max];
    s32 count = 0;
};

struct Texture {
    u32 tex = 0u;
    s32 width = 0;
    s32 height = 0;
    Subtex_Array subtexs;
};

fn texture_init(Texture* texture, Texture_Def def) -> void;
fn texture_done(Texture* texture) -> void;
fn texture_use(Texture texture, u32 unit = 0) -> void;