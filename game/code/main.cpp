#include "app.h"
#include "draw.h"
#include "graphics.h"
#include "asset.h"

fn main() -> s32 {
    
    App_Desc desc;
    desc.window.title = L"Survive 3D";
    app_init(desc);
    draw_init();
    
    Texture monk_run_texture;
    {
        Texture_Def def;
        def.kind = Texture_Kind_Multiple;
        def.subtex_size = 192;
        def.filename = "Monk.png";
        texture_init(&monk_run_texture, def);
    }

    s32 frame_count = monk_run_texture.subtexs.count;
    s32 anim_frames = 12;
    s32 curr_frame = 0;
    s32 last_frame = frame_count - 1;
    f32 frame_duration = 1.0f / (f32) anim_frames; 
    f32 frame_timer = 0.0f;

    Mesh box_stack;
    Asset_Handle mesh_shader = asset_create(Asset_Kind_Shader);
    Shader* mesh_shader_data = (Shader*) asset_get(mesh_shader);
    Shader_Def mesh_shader_def {"shader_mesh_lit.glsl"};
    shader_init(mesh_shader_data, mesh_shader_def);

    mesh_init(&box_stack, "box_stack/box_stack.obj", mesh_shader);

    set_depth_test_enabled();

    float box_spin = 0.f;

    while(app_running()) {
       
        // Frame management for 2D animations.
        //frame_timer += os_delta_time();
        //
        //while(frame_timer >= frame_duration) { // Add here currentFrame++ to each entity so that they don't always use the same frame. (when the entity is not active, the current frame wouldn't increment).
        //    frame_timer -= frame_duration;
        //    curr_frame++;
        //    if (curr_frame >= frame_count) {
        //        curr_frame = 0;
        //    }
        //}

        draw_update(os_delta_time()); // Update "camera" position.

        // Start to draw
        clear_back_buffer(); // We clear the back buffer to draw in it.

        // 2D
        draw_sprite(&monk_run_texture, curr_frame, Color.White, Mat4::transform(F32.Zero, F32.Zero, Vec3(F32.One) * 3.0f));

        // 3D
        //box_spin += 30 * os_delta_time();
        draw_mesh(&box_stack, Mat4::transform(Vec3(F32.Front) * 20.f, Vec3(0.0f, F32.to_radians(box_spin), 0.0f), Vec3(F32.One)));
        os_swap_buffers(); // Now that we drew everything we need in the back buffer we swap it with the front one to show it.
    }

    texture_done(&monk_run_texture);
    asset_free_all();
    draw_done();
    app_done();
}