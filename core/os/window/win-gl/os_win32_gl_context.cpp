#include "os_win32_gl_context.h"

#if defined (GAME_WIN) && defined(GAME_GL)

#include "os_win32_window.h"

#define GL_PROCS_NO_EXTERN
#include "os_gl.h"
#undef GL_PROCS_NO_EXTERN

#include "wglext.h"

static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{};
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{};
static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT{};

static auto resolve_wgl_functions() -> void
{
	// @Note: Old OpenGL context. Needed to load wgl extensions.
	
	// Dummy window creation.
	Window_Desc wds;
	wds.width = 0;
	wds.height = 0;
	wds.flags = Window_Desc::Flags::NONE;
	Win32_Window window(wds);
	
	// @Note: HDC stands for handle to device context.
	HDC dc = GetDC(static_cast<HWND>(window.handle()));
	PIXELFORMATDESCRIPTOR ds = {};
	
	ds.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ds.nVersion = 1;
	ds.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ds.iPixelType = PFD_TYPE_RGBA;
	ds.cColorBits = 32;
	ds.cDepthBits = 24;
	ds.iLayerType = PFD_MAIN_PLANE;
	
	s32 pf = ChoosePixelFormat(dc, &ds);
	SetPixelFormat(dc, pf, &ds);
	
	// @Note: HGLRC stands for handle to OpenGL rendering context. 
	HGLRC context = wglCreateContext(dc);
	wglMakeCurrent(dc, context);
	
	// @Note: Resolve wgl extension functions.
	wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
	wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(wglGetProcAddress("wglSwapIntervalEXT"));
	
	// @Note: Destroy the old context.
	wglMakeCurrent(dc, 0);
	wglDeleteContext(context);
}

static auto resolve_gl_proc(const char* name) -> void*
{
	void* p = (void*) wglGetProcAddress(name);

	// @Note: Illegal values returned by wglGetProcAddress.
	if (p == nullptr     ||
		p == (void*) 0x1 ||
		p == (void*) 0x2 ||
		p == (void*) 0x3 ||
		p == (void*) -1)
	{
		static HMODULE ogl = LoadLibrary(L"opengl32.dll");
		p = (void*) GetProcAddress(ogl, name);
	}

	return p;
}

static auto resolve_gl_functions()
{
#define DO_RESOLVE(_SIGNATURE, _NAME) \
	_NAME = reinterpret_cast<_SIGNATURE>(resolve_gl_proc(#_NAME));
	
	FOR_GL_FUNCTIONS(DO_RESOLVE);

#undef DO_RESOLVE
}

Win32_GL_Context::Win32_GL_Context(HWND window_handle)
{
	if (!ensuref(window_handle, "This is not a valid window handle!\n")) return;

    resolve_wgl_functions();
	
	// @Note: Modern OpenGL initialization.
	s32 pixel_forattribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0	
	};
	
	HDC dc = GetDC(window_handle);
	s32 pixel_format = 0;
	u32 num_formats = 0u;

	wglChoosePixelFormatARB(dc, pixel_forattribs, 0, 1, &pixel_format, &num_formats);

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	SetPixelFormat(dc, pixel_format, &pfd);
	
	s32 context_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		
		#ifdef GAME_DEBUG
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		#endif
		
		0 
	};
	
	HGLRC hglrc = wglCreateContextAttribsARB(dc, 0, context_attribs);
	wglMakeCurrent(dc, hglrc);
	
	m_context = hglrc;
	m_device = dc;
	
	resolve_gl_functions();

    logf("Win32 OpenGL Context created!");
}

Win32_GL_Context::~Win32_GL_Context()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_context);
    logf("Win32 OpenGL Context destroyed!");
}

auto Win32_GL_Context::swap_interval(s32 interval) -> void
{
	wglSwapIntervalEXT(interval);
}

auto Win32_GL_Context::swap_buffers() const -> void
{
    SwapBuffers(m_device);
}

#endif