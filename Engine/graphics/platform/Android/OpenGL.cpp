#include "OpenGL.hpp"
#include "Window.hpp"

#include <core/Exception.hpp>

OpenGL::~OpenGL()
{
    if (m_Context != GL_CTX{}) {
        eglDestroyContext(m_Window.display(), m_Context);
    }
}

auto OpenGL::make_current()  -> bool
{
    return eglMakeCurrent(m_Window.display(), m_Window.surface(), m_Window.surface(), m_Context);
}

auto OpenGL::find_config([[maybe_unused]] const CWindow& window) -> GL_CFG
{
    int32_t DepthBufferBits[] = { 24, 16 };

    EGLConfig config;
    EGLint numConfigs;
    EGLBoolean valid_config = false;

    for(int32_t i = 0; i < sizeof(DepthBufferBits); i++){
        static const EGLint visualAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_STENCIL_SIZE, 8,
            EGL_DEPTH_SIZE, DepthBufferBits[i],
            EGL_NONE
        };

        valid_config = eglChooseConfig(window.display(), visualAttribs, &config, 1, &numConfigs);
        if (valid_config) break;
    }

    if(!valid_config) throw Exception("Failed to choose EGL config for Android");

    return config;
}

auto OpenGL::create_context() -> GL_CTX
{
    auto display = m_Window.display();
    auto surface = m_Window.surface();

    static const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3, 
        EGL_NONE
    };

    auto context = eglCreateContext(display, m_Config, EGL_NO_CONTEXT, contextAttribs);

    if (context == EGL_NO_CONTEXT) {
        throw Exception("Failed to create EGL context (Error: {})", eglGetError());
    }

    return context;
}