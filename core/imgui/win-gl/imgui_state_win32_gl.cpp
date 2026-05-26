#include "imgui_state_win32_gl.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

struct {
    HWND  hwnd;
    HDC   main_hdc;
    HGLRC main_hglrc;
} static g_imgui;

// ------------------------------------------------------

static void hook_create_window(ImGuiViewport* viewport);
static void hook_destroy_window(ImGuiViewport* viewport);
static void hook_render_window(ImGuiViewport* viewport, void*);
static void hook_swap_buffers(ImGuiViewport* viewport, void*);

// ------------------------------------------------------

fn imgui_init_win32_gl(HWND hwnd, HDC hdc, HGLRC hglrc) -> void {
    g_imgui.hwnd = hwnd;
    g_imgui.main_hdc = hdc;
    g_imgui.main_hglrc = hglrc;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    float main_scale =
        ImGui_ImplWin32_GetDpiScaleForMonitor(
            MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_InitForOpenGL(hwnd);

    wglMakeCurrent(hdc, hglrc);
    ImGui_ImplOpenGL3_Init("#version 130");

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

        platform_io.Renderer_CreateWindow  = hook_create_window;
        platform_io.Renderer_DestroyWindow = hook_destroy_window;
        platform_io.Renderer_SwapBuffers   = hook_swap_buffers;
        platform_io.Platform_RenderWindow  = hook_render_window;
    }
}

fn imgui_done_win32_gl() -> void {    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

fn imgui_frame_init_win32_gl() -> void {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

fn imgui_frame_done_win32_gl() -> void {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        // Restore main context
        wglMakeCurrent(g_imgui.main_hdc, g_imgui.main_hglrc);
    }
}

// ------------------------------------------------------

struct WGL_WindowData {
    HDC   hDC;
    HGLRC hRC;
};

static fn setup_pixel_format(HDC hdc) -> void {

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags =
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hdc, &pfd);
    IM_ASSERT(pf != 0);

    BOOL ok = SetPixelFormat(hdc, pf, &pfd);
    IM_ASSERT(ok);
}

static fn hook_create_window(ImGuiViewport* viewport) -> void {
    IM_ASSERT(viewport->RendererUserData == nullptr);

    auto* data = IM_NEW(WGL_WindowData);
    data->hDC = GetDC((HWND)viewport->PlatformHandle);

    setup_pixel_format(data->hDC);

    data->hRC = wglCreateContext(data->hDC);
    wglShareLists(g_imgui.main_hglrc, data->hRC);
    wglMakeCurrent(data->hDC, data->hRC);

    viewport->RendererUserData = data;
}

static fn hook_destroy_window(ImGuiViewport* viewport) -> void {
    if (!viewport->RendererUserData)
        return;

    auto* data = (WGL_WindowData*)viewport->RendererUserData;

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(data->hRC);
    ReleaseDC((HWND)viewport->PlatformHandle, data->hDC);

    IM_DELETE(data);
    viewport->RendererUserData = nullptr;
}

static fn hook_render_window(ImGuiViewport* viewport, void*) -> void {
    if (auto* data = (WGL_WindowData*)viewport->RendererUserData)
        wglMakeCurrent(data->hDC, data->hRC);
}

static fn hook_swap_buffers(ImGuiViewport* viewport, void*) -> void {
    if (auto* data = (WGL_WindowData*)viewport->RendererUserData)
        SwapBuffers(data->hDC);
}