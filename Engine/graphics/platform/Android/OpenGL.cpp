#include "OpenGL.hpp"

#include "gl.hpp"
#include "Window.hpp"

#include <core/Exception.hpp>

#include <vector>

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
    return {};
}

extern auto choose_config(H_DSP display) -> EGLConfig;

auto OpenGL::create_context() -> GL_CTX
{
    auto display = m_Window.display();
    m_Config = choose_config(display);

    auto extensions = platform_extensions();
    std::vector<EGLint> attribs;

    attribs.push_back(EGL_CONTEXT_CLIENT_VERSION); attribs.push_back(MIN_REQUIRED_MAJOR_VERSION);

    if(extensions.contains("EGL_KHR_create_context")){
        attribs.push_back(EGL_CONTEXT_FLAGS_KHR); attribs.push_back(EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR);
    }

    attribs.push_back(EGL_NONE);

    auto context = eglCreateContext(display, m_Config, EGL_NO_CONTEXT, attribs.data());

    if (context == EGL_NO_CONTEXT) {
        throw Exception("Failed to create EGL context (Error: {})", eglGetError());
    }

    return context;
}

auto OpenGL::platform_extensions() const -> std::string
{
    return eglQueryString(m_Window.display(), EGL_EXTENSIONS);
}