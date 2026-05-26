# Quad Example

`main.cpp`
```cpp
#include "app.h"

s32 main() {

    app_init();

    // Counter-clock wise Quad Vertices.
    constexpr f32 QUAD_VTS[] = {
        -0.5f, -0.5f, // Vertex 0. Bottom-left. 
        +0.5f, -0.5f, // Vertex 1. Bottom-right.
        +0.5f, +0.5f, // Vertex 2. Top-right.  
        -0.5f, +0.5f, // Vertex 3. Top-left.
    };
    
    // *** VAO: Vertex Array Object ***
    
    // @Note: The parent object of our geometry.

    u32 vao = 0u;
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    
    // @Note: This will make OpenGL understand the layout of out vertices.

    // Offset in bytes from the start of the vertex.
    s32 offset = 0;

    // 0: POSITION --------------------------------------------
    
    struct {
        s32 Index = 0;
        s32 Count = 2; // Two components: X, Y.
        s32 Size  = sizeof(f32) * Count; // Size in bytes.
    } constexpr Pos;

    // Enables the Attrib index.
    glEnableVertexArrayAttrib(vao, Pos.Index);
    // Explains OpenGL the memory layout of the attribute.
    glVertexArrayAttribFormat(vao, Pos.Index, Pos.Count, GL_FLOAT, false, offset);
    // Increment the offset with the attrib size in bytes.
    offset += Pos.Size;

    // This will be explained later (sorry :p)
    glVertexArrayAttribBinding(vao, Pos.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------

    // *** VBO: Vertex Buffer Object ***
    
    // @Note: Object used to send the vertices to OpenGL.
    u32 vbo = 0u;

    // Total Vertex Size.
    constexpr u64 QUAD_VT_SIZE = Pos.Size;

    // Create the buffer, and pass the raw memory of the vertices.
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(QUAD_VTS), QUAD_VTS, GL_STATIC_DRAW);

    // Links the VBO with the VAO, in the index 0
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, QUAD_VT_SIZE);
    
    // *** EBO: Element Buffer Object ***

    // @Note: Object used to explain OpenGL how the VAO/VBO indices
    // build the triangles of our geometry.
    u32 ebo = 0u;

    constexpr u32 QUAD_ELEMS[] = {
        0u, 1u, 2u, // Triangle 1
        2u, 3u, 0u, // Triangle 2
    };
    
    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(QUAD_ELEMS), QUAD_ELEMS, GL_STATIC_DRAW);

    // Links the EBO with the VAO. 
    glVertexArrayElementBuffer(vao, ebo);

    // *** Shader Program ***
    std::string source = os_read_entire_file("shader_flat_color.glsl");
    u32 shader_flat_color = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object.

    // @Note: This is meant to send no-vertex related data
    // to the shader.
    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
    } shader_data;

    u32 ubo = 0u;
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // We want to see the quad in movement in order
        // to know if we nailed the UBO setup.
        quad_pos += Vec3(F32.Right) * os_delta_time();

        // Now we create a transform matrix with the vector.
        shader_data.transform = Mat4::transpose(Mat4::transform(quad_pos));
        
        // Also an orthographic projection matrix, to get world
        // space coordinates instead of NDC.

        f32 aspect = 16.0f / 9.0f; // This will give us an homogeneus box.
        f32 zoom   = 3.0f;         // The scale of the box.
        f32 nearpl = 0.0f;         // "Back" plane.
        f32 farpl  = 0.1f;         // "Front" plane.
        
        shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));

        // Then we pase our struct instance to the UBO and
        // tell OpenGL we're about to use it.
        glNamedBufferSubData(ubo, 0, sizeof(shader_data), &shader_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);

        // Tell OpenGL that we are going to use
        // our VAO and our shader in the next
        // draw call.
        glUseProgram(shader_flat_color);
        glBindVertexArray(vao);

        // The draw call.
        glDrawElements(GL_TRIANGLES, /* index_count */ 6, GL_UNSIGNED_INT, nullptr);

        // Swap the front and back buffers.
        os_swap_buffers();
    }

    app_done();
}
```
`shader_flat_color.glsl`

```glsl
#ifdef VERTEX_SHADER
// The Position Vertex Attribute.
layout(location = 0) in vec2 a_pos;

// The Uniform Buffer Object data.
layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
  mat4 u_transform;
};

void main() {
    // We pass it to the gl_Position vertex shader output.
    gl_Position = u_projection * u_transform * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER
// The color fragment shader output.
layout(location = 0) out vec4 o_col;

void main() {
  // We just hardcode white here.
  o_col = vec4(1.0, 1.0, 1.0, 1.0);
}
#endif
```

# Texture Example

`main.cpp`
```cpp
#include "app.h"

s32 main() {

    app_init();

    // 1. Here we added the UVs per each vertex.
    constexpr f32 QUAD_VTS[] = {
        -0.5f, -0.5f,  0.0f, 0.0f, 
        +0.5f, -0.5f,  1.0f, 0.0f,
        +0.5f, +0.5f,  1.0f, 1.0f,
        -0.5f, +0.5f,  0.0f, 1.0f,
    };
    
    // *** VAO: Vertex Array Object ***

    u32 vao = 0u;
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    
    s32 offset = 0;

    // 0: POSITION --------------------------------------------
    
    struct {
        s32 Index = 0;
        s32 Count = 2;
        s32 Size  = sizeof(f32) * Count;
    } constexpr Pos;

    glEnableVertexArrayAttrib(vao, Pos.Index);
    glVertexArrayAttribFormat(vao, Pos.Index, Pos.Count, GL_FLOAT, false, offset);
    offset += Pos.Size;
    glVertexArrayAttribBinding(vao, Pos.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------

    // 2. Then we create a vertex attribute for the UVs.

    // 1: UV --------------------------------------------------
    
    struct {
        s32 Index = 1;
        s32 Count = 2; // Two components: U, V.
        s32 Size  = sizeof(f32) * Count;
    } constexpr UV;

    glEnableVertexArrayAttrib(vao, UV.Index);
    glVertexArrayAttribFormat(vao, UV.Index, UV.Count, GL_FLOAT, false, offset);
    offset += UV.Size;
    glVertexArrayAttribBinding(vao, UV.Index, /* vbo binding */ 0u);

    // --------------------------------------------------------
    
    // *** VBO: Vertex Buffer Object ***
    
    u32 vbo = 0u;

    // 3. Add the UV attr size to the quad vertex size.
    constexpr u64 QUAD_VT_SIZE = Pos.Size + UV.Size;

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(QUAD_VTS), QUAD_VTS, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, QUAD_VT_SIZE);
    
    // *** EBO: Element Buffer Object ***

    u32 ebo = 0u;

    constexpr u32 QUAD_ELEMS[] = {
        0u, 1u, 2u, // Triangle 1
        2u, 3u, 0u, // Triangle 2
    };
    
    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(QUAD_ELEMS), QUAD_ELEMS, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);

    // *** Shader Program ***
    std::string source = os_read_entire_file("shader_texture.glsl");
    u32 shader_flat_color = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object.
    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
        s32 tex_unit = 0; // 4. Declare the texture unit as shader input data. 
    } shader_data;

    u32 ubo = 0u;
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    // 4. Create the texture.

    // *** Texture ***
    
    // Handcraft a 4 pixel image.
    //u8 image_pixels[16] = {
    //    042u, 045u, 121u, 255u, // Some blue.
    //    232u, 005u, 103u, 255u, // Some red.
    //    255u, 142u, 104u, 255u, // Some orange.
    //    244u, 234u, 188u, 255u, // Some yellow.
    //};    

    IO_Image image;
    io_image_load("hello_kitty.png", &image);
    //image.data     = image_pixels;
    //image.channels = 4; // RGBA
    //image.width    = 2; // Pixel Width.
    //image.height   = 2; // Pixel Height.

    // Create the texture object.
    u32 tex = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    
    // Check if as RGB or RGBA.
    s32 storage_format = image.channels == 4 ? GL_RGBA8 
                       : image.channels == 3 ? GL_RGB8 : 0;

    // Reserve the storage.    
    glTextureStorage2D(tex, 1, storage_format, image.width, image.height);

    // Texture config.
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check if as RGB or RGBA. (Again :S)
    s32 data_format = image.channels == 4 ? GL_RGBA 
                    : image.channels == 3 ? GL_RGB : 0;
    
    // Send the texture data to the gpu.
    glTextureSubImage2D(tex, 0, 0, 0, image.width, image.height, data_format, GL_UNSIGNED_BYTE, image.data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Which means...
    //final.rgb = src.rgb * src.a + dst.rgb * (1 - src.a)
    //final.a   = src.a

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 5. Bind the texture to the unit 0.
        glBindTextureUnit(0, tex);

        shader_data.transform = Mat4::transpose(Mat4::transform(quad_pos, F32.Zero, Vec3(F32.One) * 2.0f));
        
        f32 aspect = 16.0f / 9.0f; 
        f32 zoom   = 3.0f;        
        f32 nearpl = 0.0f;       
        f32 farpl  = 0.1f;      
        
        shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));
        shader_data.tex_unit = 0; // 6. Pass the texture unit to the shader.

        glNamedBufferSubData(ubo, 0, sizeof(shader_data), &shader_data);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);
        glUseProgram(shader_flat_color);
        
        // 7. Create the sampler array.
        s32 samplers[32];
        for (s32 i = 0; i < 32; ++i) {
            samplers[i] = i;
        }

        // 8. Pass the sampler array to the gpu via single uniform. 
        GLint location = glGetUniformLocation(shader_flat_color, "u_samplers");
        glUniform1iv(location, 32, samplers);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, /* index_count */ 6, GL_UNSIGNED_INT, nullptr);

        os_swap_buffers();
    }

    app_done();
}
```

`shader_texture.glsl`
```glsl
#ifdef VERTEX_SHADER

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_uv; // 1. Get the UV attribute.

layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
  mat4 u_transform;
  int u_tex_unit; // 2. Get the texture unit.
};

// 3. Pass the data to the fragment.
out vec2 v_uv;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * u_transform * vec4(a_pos.x, a_pos.y, 0.0, 1.0);
    // 4. Set the data.
    v_uv = a_uv; 
    v_tex_unit = u_tex_unit;
}
#endif

#ifdef FRAGMENT_SHADER

// 1. Retrieve the data from the vertex.
in vec2 v_uv;
in flat int v_tex_unit; 

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32

// 3. Alse retrieve the texture samplers from out program.
uniform sampler2D u_samplers[MAX_TEXTURES];

void main() {
  // 4. Get the current pixel color, associated
  // with our vertex, using the uv coordinates
  // and the texture unit.
  o_col = texture(u_samplers[v_tex_unit], v_uv);
}

#endif
```

# Batch Rendering Example

`main.cpp`
```cpp
#include "app.h"

u32 shader = 0;
u32 vao = 0u;
u32 vbo = 0u;
u32 ebo = 0u;
u32 ubo = 0u;

struct Sprite_Vertex {
    Vec4 pos;
    Vec2 uv;
    Vec4 tint;
    s32 tex_unit;
};

struct {
    static constexpr s32 verts_per_sprite = 4;
    static constexpr s32 elems_per_sprite = 6;
    static constexpr s32 max = 1; // Max sprites per drawcall.

    Sprite_Vertex data[max * verts_per_sprite] = {};
    s32 count = 0; // Current sprites.
} sprite_batch;

struct {
    Mat4 projection = Mat.Identity4;
} global_shader_data;

u32 white_tex = 0;

struct {
    static constexpr s32 max = 32;
    u32 data[max] = {};
    s32 count = 0;
} tex_array;

// ----------------------------------------------------------

u32 create_tex(const IO_Image& image);

void frame_init();
void draw_sprite(u32 tex, Vec3 pos, Vec3 rot, Vec3 scl, Vec4 tint); 
void frame_done();
void flush();
s32 give_tex_unit(u32 tex);

void frame_init() {

    sprite_batch.count = 0;
    tex_array.count = 0;

    // Create the white_tex if wasn't.
    if (!white_tex) {
        white_tex = create_tex(*io_image_white());
    }

    // Assign white tex to index 0.
    give_tex_unit(white_tex);    
}

void flush() {
    frame_done();
    frame_init();
};

void frame_done() {

    for (s32 i = 0; i < tex_array.count; ++i) {
        glBindTextureUnit(i, tex_array.data[i]);
    }
    
    glUseProgram(shader);

    glNamedBufferSubData(ubo, 0, sizeof(global_shader_data), &global_shader_data);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0u, ubo);

    s32 samplers[32];
    for (s32 i = 0; i < 32; ++i) {
        samplers[i] = i;
    }
    GLint location = glGetUniformLocation(shader, "u_samplers");
    glUniform1iv(location, 32, samplers);

    glNamedBufferSubData(vbo, 0, sizeof(Sprite_Vertex) * sprite_batch.verts_per_sprite * sprite_batch.count, sprite_batch.data);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, /* index_count */ sprite_batch.elems_per_sprite * sprite_batch.count, GL_UNSIGNED_INT, nullptr);
}

s32 give_tex_unit(u32 tex) {

    // Invalid tex, use white.
    if (tex == 0) {
        tex = white_tex;
    }

    // Check if we filled all the tex slots in this frame.
    if (tex_array.count >= tex_array.max) {
        flush();
    }
    
    s32 tex_unit = 0;

    // Search the tex.
    for (; tex_unit < tex_array.count; ++tex_unit) {
        if (tex_array.data[tex_unit] == tex) {
            return tex_unit; // Found.
        }
    }
    // Not found. Add to last unit.
    tex_array.data[tex_unit] = tex;
    ++tex_array.count;

    return tex_unit;
}

fn draw_sprite(u32 tex, Vec4 tint, Vec3 pos, Vec3 rot = F32.Zero, Vec3 scl = F32.One) -> void {

    if (sprite_batch.count == sprite_batch.max) {
        flush();
    }
    
    for (s32 i = 0; i < sprite_batch.verts_per_sprite; ++i) {

        constexpr Vec4 sprite_verts[] = {
            {-0.5f, -0.5f, 0.0f, 1.0f},
            {+0.5f, -0.5f, 0.0f, 1.0f},
            {+0.5f, +0.5f, 0.0f, 1.0f},
            {-0.5f, +0.5f, 0.0f, 1.0f},
        };

        constexpr Vec2 sprite_uvs[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        };

        Sprite_Vertex& vertex = sprite_batch.data[sprite_batch.count * sprite_batch.verts_per_sprite + i];
        vertex.pos      = sprite_verts[i] * Mat4::transpose(Mat4::transform(pos, rot, scl));
        vertex.uv       = sprite_uvs[i];

        vertex.tint     = tint;
        vertex.tex_unit = give_tex_unit(tex);
    }

    ++sprite_batch.count;
}

u32 create_tex(const IO_Image& image) {
    // Create the texture object.
    u32 tex = 0u;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    
    // Check if as RGB or RGBA.
    s32 storage_format = image.channels == sprite_batch.verts_per_sprite ? GL_RGBA8 
                       : image.channels == 3 ? GL_RGB8 : 0;

    // Reserve the storage.    
    glTextureStorage2D(tex, 1, storage_format, image.width, image.height);

    // Texture config.
    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Check if as RGB or RGBA. (Again :S)
    s32 data_format = image.channels == sprite_batch.verts_per_sprite ? GL_RGBA 
                    : image.channels == 3 ? GL_RGB : 0;
    
    // Send the texture data to the gpu.
    glTextureSubImage2D(tex, 0, 0, 0, image.width, image.height, data_format, GL_UNSIGNED_BYTE, image.data);

    return tex;
}

s32 main() {

    app_init();

    // *** VAO: Vertex Array Object ***
    glCreateVertexArrays(1, &vao);

    // *** Vertex Attributes ***
    s32 offset = 0;

    constexpr Data_Type sprite_attrs[] = {
        Data_Type::Float4, // Position.
        Data_Type::Float2, // UVs.
        Data_Type::Float4, // Tint Color.
        Data_Type::Int,    // Texture Unit.
    };

    s32 index = 0;
    for (Data_Type attr: sprite_attrs) {
        checkf(os_is_gl_attribute(attr), "This type does not count as gl attribute!");
        glEnableVertexArrayAttrib(vao, index);
        if (is_integer_type(attr)) {
            glVertexArrayAttribIFormat(vao, index, get_count(attr), os_to_gl(attr), offset);
        } else {
            glVertexArrayAttribFormat(vao, index, get_count(attr), os_to_gl(attr), false, offset);
        }
        glVertexArrayAttribBinding(vao, index, /* vbo binding */ 0u);
        offset += get_size(attr);
        ++index;
    }

    // *** VBO: Vertex Buffer Object ***
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(sprite_batch.data), nullptr, GL_DYNAMIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0u, vbo, 0u, sizeof(Sprite_Vertex));
    
    // *** EBO: Element Buffer Object ***
    constexpr s32 max_elems = sprite_batch.max * sprite_batch.verts_per_sprite * sprite_batch.elems_per_sprite;

    u32 all_sprite_elems[max_elems];

    // Sprite 0:  0, 1, 2, 2, 3, 0
    // Sprite 1:  4, 5, 6, 6, 7, 4
    // Sprite 2:  8, 9, 10, 10, 11, 8
    // etc...

    for (s32 i = 0; i < sprite_batch.max; ++i) {
        s32 elem_offset  = i * sprite_batch.elems_per_sprite;
        s32 vert_offset  = i * sprite_batch.verts_per_sprite;
        // Triangle 1
        all_sprite_elems[elem_offset + 0] = vert_offset + 0;
        all_sprite_elems[elem_offset + 1] = vert_offset + 1;
        all_sprite_elems[elem_offset + 2] = vert_offset + 2;
        // Triangle 2
        all_sprite_elems[elem_offset + 3] = vert_offset + 2;
        all_sprite_elems[elem_offset + 4] = vert_offset + 3;
        all_sprite_elems[elem_offset + 5] = vert_offset + 0;
    }

    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, sizeof(u32) * max_elems, all_sprite_elems, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ebo);

    // *** Shader Program ***
    std::string source = os_read_entire_file("shader_sprite.glsl");
    shader = os_create_gl_program(source);

    // *** UBO: Uniform Buffer Object ***
    glCreateBuffers(1, &ubo);
    glNamedBufferData(ubo, sizeof(global_shader_data), nullptr, GL_DYNAMIC_DRAW);
    
    // Our quad position.
    Vec3 quad_pos;

    IO_Image image;
    io_image_load("hello_kitty.png", &image);
    u32 hello_kitty_tex = create_tex(image);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (app_running())
    {
        glClear(GL_COLOR_BUFFER_BIT);
        
        f32 aspect = 16.0f / 9.0f; 
        f32 zoom   = 9.0f;        
        f32 nearpl = 0.0f;       
        f32 farpl  = 0.1f;      
        
        global_shader_data.projection = Mat4::transpose(Mat4::orthographic(aspect, zoom, nearpl, farpl));

        constexpr s32 max = 900000;
        static Vec3* positions = new Vec3[max];
        static Vec4* colors = new Vec4[max];
        static s32 count = 0;
        static f32 timer = 0.0f;

        timer += os_delta_time();

        if (timer >= 0.001f) {
            if (count < max) {
                Vec2 pos = Vec2::random({ -16.f, 9.f }, { 16.f, -9.f });
                positions[count] = { pos.x, pos.y, 0.0f };
                colors[count] = Vec4::random(Color.Black, Color.White);
                count++;
            }
            timer = 0.0f;
        }

        frame_init();
        for (s32 i = 0; i < count; ++i) {
            draw_sprite(hello_kitty_tex, colors[i], positions[i]);
        }
        frame_done();

        imgui_frame_init();

        static f32 time = 0.0f;
        time += os_delta_time();

        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", os_av_fps());
        ImGui::Text("Hello Kitty Count: %i", count);
        ImGui::Text("Time: %f", time);
        ImGui::End();

        imgui_frame_done();


        os_swap_buffers();
    }

    app_done();
}
```

`shader_sprite.glsl`
```glsl
#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_pos;
layout(location = 1) in vec2 a_uv; 
layout(location = 2) in vec4 a_tint;
layout(location = 3) in int a_tex_unit;

layout(std140, binding = 0) uniform Shader_Data {
  mat4 u_projection;
};

out vec2 v_uv;
out vec4 v_tint;
out flat int v_tex_unit; 

void main() {
    gl_Position = u_projection * vec4(a_pos.x, a_pos.y, a_pos.z, a_pos.w);

    v_uv = a_uv; 
    v_tint = a_tint; 
    v_tex_unit = a_tex_unit;
}
#endif

#ifdef FRAGMENT_SHADER

// 1. Retrieve the data from the vertex.
in vec2 v_uv;
in vec4 v_tint;
in flat int v_tex_unit; 

layout(location = 0) out vec4 o_col;

#define MAX_TEXTURES 32

uniform sampler2D u_samplers[MAX_TEXTURES];

void main() {
  o_col = texture(u_samplers[v_tex_unit], v_uv) * v_tint;
}

#endif
```