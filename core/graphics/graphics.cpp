#include "graphics.h"
#include "os_gl.h"
#include "os_core.h"
#include "io_image.h"

fn clear_back_buffer(Vec4 color) -> void {
    os_clear_color_gl(color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

fn set_blend_enabled(bool enabled) -> void {
    if (enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

fn set_viewport(s32 viewport_x, s32 viewport_y) -> void {
    glViewport(0, 0, viewport_x, viewport_y);
}

fn set_depth_test_enabled(bool enabled) -> void {
    if (enabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

fn vertex_buffer_init(Vertex_Buffer* obj, Vertex_Buffer_Def def) -> void {

    auto &[vao, ebo, vbo, _] = *obj;

    glCreateVertexArrays(1, &vao);
    
    s32 offset = 0;
    
    // Process the attributes.
    for (s32 i = 0; i < def.attrs.count; ++i) {
        Data_Type attr = def.attrs.data[i];
        if (!os_is_gl_attribute(attr)) {
            continue;
        }
        glEnableVertexArrayAttrib(vao, i);
        if (!is_integer_type(attr)) {
            glVertexArrayAttribFormat(vao, i, get_count(attr), os_to_gl(attr), false, offset);
        } else {
            glVertexArrayAttribIFormat(vao, i, get_count(attr), os_to_gl(attr), offset);
        }
        glVertexArrayAttribBinding(vao, i, /* vbo binding */ 0u);
        offset += get_size(attr);
    }

    // Create the vertex buffer and send the data.
    glCreateBuffers(1, &vbo);
    GLenum usage = def.verts.data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
    glNamedBufferData(vbo, def.verts.size, def.verts.data, usage);
    glVertexArrayVertexBuffer(vao, /* vbo binding */ 0, vbo, /* offset */ 0, def.verts.size / def.verts.count);

    // Create the element buffer.
    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, def.elems.count * sizeof(u32), def.elems.data, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);
    obj->elem_count = def.elems.count;
}

fn vertex_buffer_done(Vertex_Buffer* obj) -> void {
    auto &[vao, ebo, vbo, _] = *obj;
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    *obj = {};
}

fn vertex_buffer_draw(Vertex_Buffer obj, u32 count, u32 index_offset) -> void {
    checkf(obj.vao != 0u, "Error! This is not a valid Vertex Array!");
    glBindVertexArray(obj.vao);
    glDrawElements(GL_TRIANGLES, count == 0 ? obj.elem_count : count, GL_UNSIGNED_INT, (void*) (index_offset * sizeof(u32)));
}

fn shader_init(Shader* shader, Shader_Def def) -> void {
    std::string source = os_read_entire_file(def.filename);
    checkf(!source.empty(), "Error! This is not a valid Vertex Array!");
    shader->pgm = os_create_gl_program(source);
}

fn shader_done(Shader* shader) -> void {
    glDeleteProgram(shader->pgm);
    *shader = {};
}

fn shader_use(Shader shader) -> void {
    checkf(shader.pgm != 0, "Error! This is not a valid Shader!");
    glUseProgram(shader.pgm);
}

fn shader_set_param(Shader shader, std::string_view name, const s32* data, s32 count) -> void {
    checkf(shader.pgm != 0, "Error! This is not a valid Shader!");
    GLint location = glGetUniformLocation(shader.pgm, name.data());
    glUniform1iv(location, count, data);
}

fn global_buffer_init(Global_Buffer* obj, Global_Buffer_Def def) -> void {
    glCreateBuffers(1, &obj->gbo);
    glNamedBufferData(obj->gbo, def.size, nullptr, GL_DYNAMIC_DRAW);
    obj->size = def.size;    
}

fn global_buffer_done(Global_Buffer* obj) -> void {
    glDeleteBuffers(1, &obj->gbo);
    *obj = {};
}

fn global_buffer_use(Global_Buffer obj, u32 index) -> void {
    checkf(obj.gbo != 0u, "Error! This is not a valid Global Buffer!");
    glBindBufferBase(GL_UNIFORM_BUFFER, index, obj.gbo);
}

fn global_buffer_update(Global_Buffer obj, const void* data) -> void {
    checkf(obj.gbo != 0u, "Error! This is not a valid Global Buffer!");
    glNamedBufferSubData(obj.gbo, /* offset */ 0, obj.size, data);
}

fn texture_init(Texture* texture, Texture_Def def) -> void {
    
    IO_Image image_buff;
    const IO_Image* image = nullptr;
    if (def.image) {
       image = def.image; 
    } else {
        io_image_load(def.filename, &image_buff);
        image = &image_buff;
    }
    checkf(io_image_valid(*image), "Error! This is not a valid Image!");

    u32& tex = texture->tex;
    texture->width = image->width;
    texture->height = image->height;

    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    
    // Check if as RGB or RGBA.
    s32 storage_format = image->channels == 4 ? GL_RGBA8 
                       : image->channels == 3 ? GL_RGB8 : 0;

    // Reserve the storage.    
    glTextureStorage2D(tex, 1, storage_format, image->width, image->height);

    // Texture config.
    GLenum filter = def.filter == Texture_Filter_Nearest ? GL_NEAREST : 
                    def.filter == Texture_Filter_Linear  ? GL_LINEAR  : 0;

    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filter);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check if as RGB or RGBA. (Again :S)
    s32 data_format = image->channels == 4 ? GL_RGBA 
                    : image->channels == 3 ? GL_RGB : 0;
    
    // Send the texture data to the gpu.
    glTextureSubImage2D(tex, 0, 0, 0, image->width, image->height, data_format, GL_UNSIGNED_BYTE, image->data);

    if (io_image_valid(image_buff)) {
        io_image_free(&image_buff);
    }

    // Build the subtex info
    switch(def.kind) {
        case Texture_Kind_Multiple: {
            if (!ensuref(def.subtex_size <= texture->width && def.subtex_size <= texture->height, "Error! Invalid subtex_size!")) {
                break;
            }
            s32 x_count = texture->width  / def.subtex_size;
            s32 y_count = texture->height / def.subtex_size;
            s32 subtex_count = x_count * y_count;

            if (!ensuref(subtex_count <= texture->subtexs.max, "Error! Subtex count is %i. Max allowed is %i", subtex_count, texture->subtexs.max)) {
                break;
            }

            texture->subtexs.count = subtex_count;

            for (s32 i = 0; i < subtex_count; ++i) {
                s32 x = i % x_count;                
                s32 y = i / x_count;
                
                auto &subtex = texture->subtexs.data[i];
                subtex.width = def.subtex_size;
                subtex.height = def.subtex_size;
                subtex.x = def.subtex_size * x;
                subtex.y = def.subtex_size * y;
            }
        }
        case Texture_Kind_Single:
        default: {
            // We don't need to do anything here.
        }
    }
}

fn texture_done(Texture* texture) -> void {
    glDeleteTextures(1, &texture->tex);
    *texture = {};
}

fn texture_use(Texture texture, u32 unit) -> void {
    checkf(texture.tex != 0, "Error! This is not a valid Texture!");
    glBindTextureUnit(unit, texture.tex);
}