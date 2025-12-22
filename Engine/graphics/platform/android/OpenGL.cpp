#include "OpenGL.hpp"
#include "Window.hpp"

OpenGL::~OpenGL()
{
    // if (m_Context > 0) {
        // glXDestroyContext(m_Context); needs display
    // }
}

auto OpenGL::make_current_opengl([[maybe_unused]] const CWindow& window)  -> bool
{
    return glXMakeCurrent(window.display(), window.handle(), m_Context);
}

auto OpenGL::create_opengl_context([[maybe_unused]] const CWindow& window) -> GL_CTX
{
    auto display = window.display();
    static int32_t visualAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, gl::ChannelBits,
        EGL_GREEN_SIZE, gl::ChannelBits,
        EGL_RED_SIZE, gl::ChannelBits,
        EGL_ALPHA_SIZE, gl::AlphaBits,
        EGL_DEPTH_SIZE, gl::DepthBufferBits,
        EGL_NONE
    };
    
    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        throw Exception("Failed to get framebuffer config.");
    }

    XVisualInfo* visInfo = glXGetVisualFromFBConfig(display, fbc[0]);
    if (!visInfo) {
        XFree(fbc);
        throw Exception("Failed to get visual info.");
    }

    int32_t contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, gl::OPENGL_MAJOR_VERSION,
        GLX_CONTEXT_MINOR_VERSION_ARB, gl::OPENGL_MINOR_VERSION,
        #ifdef DEBUG
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB | GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        #endif
        0
    };

    glXCreateContextAttribsARB = (decltype(glXCreateContextAttribsARB))glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");

    auto context = glXCreateContextAttribsARB(display, fbc[0], nullptr, True, contextAttribs);
    XFree(fbc);
    XFree(visInfo);

    if (!context) {
        throw Exception("Failed to create GLX context.");
    }

    return context;
}