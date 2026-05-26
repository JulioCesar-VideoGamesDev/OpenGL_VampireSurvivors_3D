#include "app.h"

#if defined(GAME_DEBUG) && defined(GAME_GL)

// @Note: This is not in the GL context because it's platform independent. (I guess)

static fn APIENTRY _gl_debug_callback(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message, 
    const void* userParam
) -> void
{
    logf("[GL DEBUG] %s", message);
}

#endif

struct App {
    bool is_init = false;
    bool quit = false;
} g_app;

fn app_init(App_Desc ds) -> bool {

    if (g_app.is_init) {
        app_done();
    }
    
    os_set_working_dir(ds.working_dir);
    os_input_init(ds.input);
    os_window_init(ds.window);
    
    if (ds.init_imgui) {
        imgui_init(os_window());
    }
    
    io_audio_init();
    
#if defined(GAME_DEBUG) && defined(GAME_GL)
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_gl_debug_callback, nullptr);
#endif

    //glBindTexture(GL_TEXTURE_2D, 999999); // @Note: Uncomment to check if the errors work.

    g_app.is_init = true;
    return g_app.is_init;
}

fn app_done() -> void {

    if (!g_app.is_init) {
        return;
    }

    io_audio_init();
    imgui_done();
    os_window_done();
    os_input_done();
    g_app = {};
}

fn app_running() -> bool {

    static fn quit = []() {
        g_app.quit = true;
        if (imgui_is_init()) {
            imgui_force_save_ini();
        }
    };

    os_poll_events();
    os_time_step();

    for (const auto& event: os_events_this_frame()) {
        if (event.type == Input_Event::Quit) {
            quit();    
        }
        #if GAME_DEBUG
        if (event.key_code == Key_Code::Escape) {
            quit();
        }
        #endif      
    }
    return !g_app.quit;
}