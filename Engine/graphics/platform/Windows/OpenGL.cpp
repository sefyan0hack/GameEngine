#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Log.hpp>

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

    if (!SetPixelFormat(surface, m_Config, nullptr)) { // TODO: nullptr in 3 arg may caus prblms
        throw Exception("Failed to set the pixel format. : {}", GetLastError());
    }

    GL_CTX dummy_context = nullptr;

    dummy_context = wglCreateContext(surface);
    if (!dummy_context) {
        throw Exception("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    return dummy_context;
}