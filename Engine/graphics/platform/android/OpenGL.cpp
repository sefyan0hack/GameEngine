#include "OpenGL.hpp"
#include "Window.hpp"

OpenGL::~OpenGL()
{
    // if (m_Context > 0) {
        // glXDestroyContext(m_Context); needs display
    // }
}

auto OpenGL::make_current_opengl()  -> bool
{
    return eglMakeCurrent(m_Window.display(), m_Window.surface(), m_Window.surface(), m_Context);
}

auto OpenGL::create_opengl_context() -> GL_CTX
{
    auto display = m_Window.display();
    auto surface = m_Window.surface();

    static const EGLint visualAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, gl::ChannelBits,
        EGL_GREEN_SIZE, gl::ChannelBits,
        EGL_RED_SIZE, gl::ChannelBits,
        EGL_ALPHA_SIZE, gl::AlphaBits,
        EGL_DEPTH_SIZE, gl::DepthBufferBits,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;

    if (!eglChooseConfig(display, visualAttribs, &config, 1, &numConfigs) || numConfigs <= 0) {
        throw Exception("Failed to choose EGL config for Android");
    }

    static const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3, 
        EGL_NONE
    };

    auto context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);

    if (context == EGL_NO_CONTEXT) {
        throw Exception("Failed to create EGL context (Error: {})", eglGetError());
    }

    return context;
}