#include "draw.h"
#include "graphics.h"
#include "io_image.h"
#include "os_input.h"
#include "io_model.h"

// I define all the structs that I will need for the shaders.

struct {
    Vertex_Buffer vbo;
    Shader shader;
} quad;

struct light_Data {
  Vec4 color;
  Vec4 pos;
  Vec4 view_pos;
} light_data; // CONVERTIR EN LISTA DE LUCES PARA TENER MÁS DE UNA Y QUE EL SHADER LAS UPDATEE TODA
// In order to have multiple lights, ig I will need to make a list of light_data

constexpr s32 MAX_LIGHTS = 32;

struct Light_Buffer {
  light_Data lights[MAX_LIGHTS];
  Vec4 light_count;
};


struct {
    Camera camera;
    Free_Look camera_controller;

    s32 viewport_x = 1270;
    s32 viewport_y = 720;

    Texture white;

    Global_Buffer gbo;
    Global_Buffer gbo_light_data;

    struct {
        Mat4 projection = Mat.Identity4;
        Mat4 transform = Mat.Identity4;
        Vec2 uv_size = F32.One;
        Vec2 uv_offset = F32.Zero;
        Vec4 tint = Color.White;
        s32  tex_unit = 0;

        //Vec4 camera_pos;
    } global_data;

    Light_Buffer light_buffer;

} scene;

Shader mesh_shader;

fn draw_init() -> void {

    // Quad init.
    {
        constexpr f32 verts[] = {
            -0.5f, -0.5f,  0.0f, 0.0f, 
            +0.5f, -0.5f,  1.0f, 0.0f,
            +0.5f, +0.5f,  1.0f, 1.0f,
            -0.5f, +0.5f,  0.0f, 1.0f,
        };
        constexpr u32 elems[] = {
            0u, 1u, 2u, // Triangle 1
            2u, 3u, 0u, // Triangle 2
        };
        constexpr Data_Type attrs[] = {
            Data_Type::Float2,
            Data_Type::Float2,
        };

        Vertex_Buffer_Def vbo_def = {
            { verts, sizeof(verts), /* vertex count */ 4 },
            { elems, /* element count */ 6 },
            { attrs, /* attribute count * */ 2 },
        };
        vertex_buffer_init(&quad.vbo, vbo_def);
        
        const char* shader_filename = "shader_sprite.glsl";
        shader_init(&quad.shader, {shader_filename});

        global_buffer_init(&scene.gbo, { sizeof(scene.global_data) });
        global_buffer_init(&scene.gbo_light_data, { sizeof(Light_Buffer) }); // In the future pass a list of all light_data in sizeof
    }

    Texture_Def def;
    def.image = io_image_white();
    texture_init(&scene.white, def);

    // Models init
    {
        scene.camera = Camera(Camera::Perspective);
        scene.camera_controller.init(&scene.camera);
        const char* shader_filename = "shader_mesh.glsl";
        shader_init(&mesh_shader, {shader_filename});
    }

    // Lights
    /*Light_Data* light = &scene.light_buffer.lights[scene.light_buffer.light_count++];

    light->pos = Vec4(x, y, z, 1.0f);
    light->color = Vec4(r, g, b, intensity);*/
}

fn draw_sprite(const Texture* tex, s32 frame, Vec4 tint, const Mat4& transform) -> void {

    set_blend_enabled();
    
    if (tex && tex->subtexs.count) {
        if (!ensuref(frame < tex->subtexs.count, "Error! subtex %i does not exist!", 20)) {
            return;
        }
    
        auto& subtex = tex->subtexs.data[frame];

        scene.global_data.uv_size = { 
            (f32) subtex.width  / (f32) tex->width, 
            (f32) subtex.height / (f32) tex->height 
        }; 
        
        scene.global_data.uv_offset = { 
            (f32) subtex.x / (f32) tex->width, 
            1.0f - ((f32)subtex.y + subtex.height) / tex->height
        };
    }

    scene.global_data.transform = Mat4::transpose(transform);
    scene.global_data.tint = tint;

    texture_use(scene.white, 0u);

    if (tex) {
        scene.global_data.tex_unit = 1;
        texture_use(*tex, /* unit */ 1u);
    } else {
        scene.global_data.tex_unit = 0;
    }

    shader_use(quad.shader);

    s32 samplers[32];
    for (s32 i = 0; i < 32; ++i) {
        samplers[i] = i;
    }

    shader_set_param(quad.shader, "u_samplers", samplers, 32);

    global_buffer_update(scene.gbo, &scene.global_data);
    global_buffer_use(scene.gbo);

    vertex_buffer_draw(quad.vbo);
}

fn draw_sprite(Vec4 tint, const Mat4& transform) -> void {
    draw_sprite(nullptr, 0, tint, transform);
}

fn draw_done() -> void {
    global_buffer_done(&scene.gbo_light_data);
    global_buffer_done(&scene.gbo);
    vertex_buffer_done(&quad.vbo);
    shader_done(&quad.shader);
    shader_done(&mesh_shader);
}

fn draw_update(f32 dt) -> void {
    
    bool use_editor_camera = os_key_down(Key_Code::Mouse_Right);

    for (const auto& event : os_events_this_frame()) {
        if (use_editor_camera && event.type == Input_Event::Mouse_Move) {
            scene.camera_controller.rotate((f32)event.mouse_delta_x, (f32)event.mouse_delta_y);
            continue;
        }
        if (event.type == Input_Event::Window) {
            scene.viewport_x = event.window_x;
            scene.viewport_y = event.window_y;
            continue;
        }
    }

    if (use_editor_camera) {
        os_set_cursor_mode(Cursor_Mode::Hidden);
        if (os_key_down('W')) scene.camera_controller.move_forward(dt);
        if (os_key_down('S')) scene.camera_controller.move_backward(dt);
        if (os_key_down('A')) scene.camera_controller.move_left(dt);
        if (os_key_down('D')) scene.camera_controller.move_right(dt);
        if (os_key_down(Key_Code::Space)) scene.camera_controller.move_up(dt);
        if (os_key_down(Key_Code::Shift)) scene.camera_controller.move_down(dt);
    } else {
        os_set_cursor_mode(Cursor_Mode::Default);
    }

    scene.camera.update_matrix(scene.viewport_x, scene.viewport_y);
    scene.global_data.projection = scene.camera.matrix();
    set_viewport(scene.viewport_x, scene.viewport_y);
}

fn mesh_init(Mesh* mesh, std::string_view filename, Asset_Handle shader, bool normals_as_colors) -> void {
    IO_Model model;
    model.normals_as_colors = normals_as_colors;
    io_model_load(filename, &model);
    
    constexpr Data_Type attrs[] = {
        Data_Type::Float3,
        Data_Type::Float2,
        Data_Type::Float3,
        Data_Type::Float4,
    };
    
    Vertex_Buffer_Def def;
    def.attrs.count = 4;
    def.attrs.data = attrs;
    def.verts.count = model.vertices.count;
    def.verts.data = model.vertices.data;
    def.verts.size = sizeof(IO_Model_VTX) * def.verts.count;
    def.elems.count = model.elems.count;
    def.elems.data = model.elems.data;
    vertex_buffer_init(&mesh->vbo, def);

    for(auto& shape: model.shapes) {

        Submesh& submesh = append(&mesh->submeshes);
        submesh.elem_count = shape.index_count;                
        submesh.elem_offset = shape.index_offset;

        if (shader.kind == Asset_Kind_Shader && shader.value.index != 0) {

            Asset_Handle material = asset_create(Asset_Kind_Material);
            submesh.material = material;
            Material* material_data = (Material*) asset_get(material);
            material_data->shader = shader;
            
            auto& textures = shape.textures;
            std::string diffuse_name = textures.diffuse.empty() ? textures.ambient : textures.diffuse;

            if (!diffuse_name.empty()) {
                Asset_Handle diffuse = asset_create(Asset_Kind_Texture);
                Texture *diffuse_data = (Texture *)asset_get(diffuse);
                Texture_Def diffuse_def;
                std::string model_dirpath = model.dirpath;
                diffuse_def.filename = model_dirpath.append("\\").append(diffuse_name);
                texture_init(diffuse_data, diffuse_def);
                material_data->texture_diffuse = diffuse;
            }

        }                
    }

    io_model_done(&model);
}

fn mesh_done(Mesh* mesh) -> void {
    reset(&mesh->submeshes);
    vertex_buffer_done(&mesh->vbo);
}

fn draw_mesh(const Mesh* mesh, const Mat4& transform) -> void {

    scene.global_data.transform = Mat4::transpose(transform);
    scene.global_data.tex_unit = 0;
    
    s32 samplers[32];
    for (s32 i = 0; i < 32; ++i) {
        samplers[i] = i;
    }

    shader_set_param(mesh_shader, "u_samplers", samplers, 32);

    global_buffer_update(scene.gbo, &scene.global_data);
    global_buffer_use(scene.gbo, 0);

	// Set up light data (for now, just a single white light at the vector zero and pointing to the camera position)
    /*light_data.color = Vec4(Color.White);
    light_data.pos = Vec4(0, 0, 0, 0);
    light_data.view_pos = Vec4(scene.camera.pos.x, scene.camera.pos.y, scene.camera.pos.z, 1.0);*/

    scene.light_buffer.light_count.x = 3;

    // Light 1
    scene.light_buffer.lights[0].color = Vec4(1, 1, 1, 1); // White
    scene.light_buffer.lights[0].pos = Vec4(0, 100, 0, 1);

    scene.light_buffer.lights[0].view_pos =
      Vec4(scene.camera.pos.x,
        scene.camera.pos.y,
        scene.camera.pos.z,
        1.0f);
    
    // Light 2
    scene.light_buffer.lights[1].color = Vec4(1, 0, 0, 1);
    scene.light_buffer.lights[1].pos = Vec4(0, 0, 0, 1);

    scene.light_buffer.lights[1].view_pos =
      Vec4(scene.camera.pos.x,
        scene.camera.pos.y,
        scene.camera.pos.z,
        1.0f);

    // Light 3
    scene.light_buffer.lights[2].color = Vec4(0, 1, 0, 1);
    scene.light_buffer.lights[2].pos = Vec4(0, 0, 100, 1);

    scene.light_buffer.lights[2].view_pos =
      Vec4(scene.camera.pos.x,
        scene.camera.pos.y,
        scene.camera.pos.z,
        1.0f);

    //printf("light_count = %f\n", scene.light_buffer.light_count.x);
    global_buffer_update(scene.gbo_light_data, &scene.light_buffer);
    global_buffer_use(scene.gbo_light_data, 1);
    
    for (auto& submesh: mesh->submeshes) {

        // Set material properties for each submesh.
        Material* material_data = (Material*) asset_get(submesh.material);
        Shader* shader_data = (Shader*) asset_get(material_data->shader);
        Texture* diffuse_data = (Texture*) asset_get(material_data->texture_diffuse);

        texture_use(*diffuse_data, 0u);
        shader_use(*shader_data);
        vertex_buffer_draw(mesh->vbo, submesh.elem_count, submesh.elem_offset);
    }

}