#include "OpenGL.hpp"
#include "Window.hpp"

OpenGL::~OpenGL()
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(m_Context);
}


auto OpenGL::make_current_opengl()  -> bool
{
    return wglMakeCurrent(m_Window.surface(), m_Context);
}

auto OpenGL::create_opengl_context() -> GL_CTX
{
    auto surface = m_Window.surface();

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        static_cast<BYTE>(gl::ChannelBits * 3),  // RGB bits
        static_cast<BYTE>(gl::AlphaBits),         // Alpha bits
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        static_cast<BYTE>(gl::DepthBufferBits),   // Depth buffer
        static_cast<BYTE>(gl::StencilBufferBits), // Stencil buffer
        0,
        PFD_MAIN_PLANE,
        0, 0, 0
    };
    auto pixel_format = ChoosePixelFormat(surface, &pfd);
    if (!pixel_format) {
        throw Exception("Failed to find a suitable pixel format. : {}", GetLastError());
    }
    if (!SetPixelFormat(surface, pixel_format, &pfd)) {
        throw Exception("Failed to set the pixel format. : {}", GetLastError());
    }

    GL_CTX dummy_context = nullptr;

    dummy_context = wglCreateContext(surface);
    if (!dummy_context) {
        throw Exception("Failed to create a dummy OpenGL rendering context. : {}", GetLastError());
    }

    if (!wglMakeCurrent(surface, dummy_context)) {
        throw Exception("Failed to activate dummy OpenGL rendering context. : {}", GetLastError());
    }

    gl::GetExtensionsStringARB  = gl::GetProcAddress<decltype(gl::GetExtensionsStringARB)>("wglGetExtensionsStringARB");
    gl::CreateContextAttribsARB = gl::GetProcAddress<decltype(gl::CreateContextAttribsARB)>("wglCreateContextAttribsARB");

    if (!gl::GetExtensionsStringARB){
        throw Exception("Failed to load wglGetExtensionsStringARB. : {}", GetLastError());
    }

    if(!gl::CreateContextAttribsARB) {
        throw Exception("Failed to load wglCreateContextAttribsARB. : {}", GetLastError());
    }

    int32_t gl_attribs[] = { 
        WGL_CONTEXT_MAJOR_VERSION_ARB, gl::OPENGL_MAJOR_VERSION,
        WGL_CONTEXT_MINOR_VERSION_ARB, gl::OPENGL_MINOR_VERSION,
    #ifdef DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB | WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    #endif
        0,
    };


    GL_CTX opengl_context = nullptr;
    if (nullptr == (opengl_context = gl::CreateContextAttribsARB(surface, nullptr, gl_attribs))) {
        m_Context = nullptr;

        if (GetLastError() == ERROR_INVALID_VERSION_ARB){ // ?
            throw Exception("Unsupported GL Version {}.{}", gl::OPENGL_MAJOR_VERSION, gl::OPENGL_MINOR_VERSION);
        }
        throw Exception("Failed to create the final rendering context!");
    }

    wglDeleteContext(dummy_context);
 
    return opengl_context;
}