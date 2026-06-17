#include "OpenGL.hpp"
#include "Window.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>

#include <cstring>

OpenGL::~OpenGL()
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_Context);
}

auto OpenGL::make_current_opengl()  -> bool
{
    return wglMakeCurrent(m_Window.surface(), m_Context);
}

auto OpenGL::find_config([[maybe_unused]] const CWindow& window) -> GL_CFG
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        static_cast<BYTE>(8 * 3),  // RGB bits
        static_cast<BYTE>(8),         // Alpha bits
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        static_cast<BYTE>(24),   // Depth buffer
        static_cast<BYTE>(8), // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0, 0, 0
    };

    auto pixel_format = ChoosePixelFormat(window.surface(), &pfd);

    if (!pixel_format) {
        throw Exception("Failed to find a suitable pixel format. : {}", GetLastError());
    }

    return pixel_format;
}

auto OpenGL::create_opengl_context() -> GL_CTX
{
    auto surface = m_Window.surface();

    PIXELFORMATDESCRIPTOR desc = { .nSize = sizeof(desc) };

    if(!DescribePixelFormat(surface, m_Config, sizeof(desc), &desc))
        throw Exception("Failed Describe pixel format. : {}", GetLastError());

    if (!SetPixelFormat(surface, m_Config, &desc))
        throw Exception("Failed to set the pixel format. : {}", GetLastError());

    GL_CTX dummy_context = nullptr;

    dummy_context = wglCreateContext(surface);
    if (!dummy_context) {
        throw Exception("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    wglMakeCurrent(surface, dummy_context);

    {
    // extension_supported shuld work caus gl ctx it valid here
        bool is_WGL_ARB_framebuffer_sRGB = extension_supported("WGL_ARB_framebuffer_sRGB");
        bool is_WGL_ARB_multisample = extension_supported("WGL_ARB_multisample");

        int attribs[] =
        {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
            WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB,     24,
            WGL_DEPTH_BITS_ARB,     24,
            WGL_STENCIL_BITS_ARB,   8,

            //WGL_ARB_framebuffer_sRGB extension
            WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, (is_WGL_ARB_framebuffer_sRGB ? GL_TRUE : FALSE),

            // WGL_ARB_multisample extension
            WGL_SAMPLE_BUFFERS_ARB, (is_WGL_ARB_multisample ? 1 : 0),
            WGL_SAMPLES_ARB,        (is_WGL_ARB_multisample ? 4 : 0),

            0,
        };

        int format{};
        UINT formats{};

        static auto wglChoosePixelFormatARB_ = [](){
            auto r = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
            if (r) return r;
            else throw Exception("Failed to load wglChoosePixelFormatARB. (maybe not supported): {}", GetLastError());
        }();

        if (!wglChoosePixelFormatARB_(surface, attribs, nullptr, 1, &format, &formats) || formats == 0)
            throw Exception("OpenGL does not support required pixel format.");

        PIXELFORMATDESCRIPTOR desc2 = { .nSize = sizeof(desc2) };

        if(!DescribePixelFormat(surface, format, sizeof(desc2), &desc2))
            throw Exception("Failed Describe pixel format. : {}", GetLastError());

        if (!SetPixelFormat(surface, format, &desc2))
            throw Exception("Failed to set the pixel format. : {}", GetLastError());
    }

    // modern OpenGL context
    {
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, gl::MIN_REQUIRED_MAJOR_VERSION,
            WGL_CONTEXT_MINOR_VERSION_ARB, gl::MIN_REQUIRED_MINOR_VERSION,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,         m_Debug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
            0, 0
        };

        static auto wglCreateContextAttribsARB_ = [](){
            auto r = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
            if (r) return r;
            else throw Exception("Failed to load wglCreateContextAttribsARB. (maybe not supported): {}", GetLastError());
        }();

        auto modern_context = wglCreateContextAttribsARB_(surface, nullptr, attribs);

        if(!modern_context){
            throw Exception("Cannot create OpenGL {}.{} not supported?", gl::MIN_REQUIRED_MAJOR_VERSION, gl::MIN_REQUIRED_MINOR_VERSION);
        } else {
            wglDeleteContext(dummy_context);
            dummy_context = modern_context;
        }
    }

    return dummy_context;
}