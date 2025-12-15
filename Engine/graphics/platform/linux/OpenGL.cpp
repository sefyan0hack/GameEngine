#include "OpenGL.hpp"

OpenGL::~OpenGL()
{
    // if (m_Context > 0) {
        // glXDestroyContext(m_Context); needs surface
    // }
}

auto OpenGL::make_current_opengl([[maybe_unused]] const CWindow& window)  -> bool
{
    return glXMakeCurrent(window.surface(), window.handle(), m_Context);
}

auto OpenGL::create_opengl_context([[maybe_unused]] const CWindow& window) -> GL_CTX
{
    auto surface = window.surface();
    static int32_t visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       gl::ChannelBits,
        GLX_GREEN_SIZE,     gl::ChannelBits,
        GLX_BLUE_SIZE,      gl::ChannelBits,
        GLX_ALPHA_SIZE,     gl::AlphaBits,
        GLX_DEPTH_SIZE,     gl::DepthBufferBits,
        0
    };
    
    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(surface, DefaultScreen(surface), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        throw Exception("Failed to get framebuffer config.");
    }

    XVisualInfo* visInfo = glXGetVisualFromFBConfig(surface, fbc[0]);
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

    auto context = glXCreateContextAttribsARB(surface, fbc[0], nullptr, True, contextAttribs);
    XFree(fbc);
    XFree(visInfo);

    if (!context) {
        throw Exception("Failed to create GLX context.");
    }

    return context;
}