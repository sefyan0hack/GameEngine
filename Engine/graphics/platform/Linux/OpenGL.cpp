#include "OpenGL.hpp"
#include "Window.hpp"

OpenGL::~OpenGL()
{
    if (m_Context != GL_CTX{}) {
        glXDestroyContext(m_Window.display(), m_Context);
    }

    if(!m_Config) XFree(m_Config);
}

auto OpenGL::make_current_opengl()  -> bool
{
    return glXMakeCurrent(m_Window.display(), m_Window.handle(), m_Context);
}

auto OpenGL::find_config([[maybe_unused]] const CWindow& window) -> GL_CFG
{
    static int32_t visualAttribs[] = {
        GLX_X_RENDERABLE,  true,
        GLX_DOUBLEBUFFER,  true,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_STENCIL_SIZE,   8,
        GLX_DEPTH_SIZE,     24,
        0
    };
    
    int32_t fbcount;
    GLXFBConfig* fbc = glXChooseFBConfig(window.display(), DefaultScreen(window.display()), visualAttribs, &fbcount);
    if (!fbc || fbcount == 0) {
        throw Exception("Failed to get framebuffer config.");
    }

    return fbc;
}

auto OpenGL::create_opengl_context() -> GL_CTX
{
    auto display = m_Window.display();

    int32_t contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, gl::OPENGL_MAJOR_VERSION,
        GLX_CONTEXT_MINOR_VERSION_ARB, gl::OPENGL_MINOR_VERSION,
        #ifdef DEBUG
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB | GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        #endif
        0
    };

    static auto CreateContextAttribsARB_ = [](){
        auto r = gl::GetProcAddress<PFNGLXCREATECONTEXTATTRIBSARBPROC>("glXCreateContextAttribsARB");
        if (r) {
            return r;
        } else {
            throw Exception("Failed to load glXCreateContextAttribsARB. : {}", GetLastError());
        }
    }();

    auto context = CreateContextAttribsARB_(display, m_Config[0], nullptr, True, contextAttribs);

    if (!context) {
        throw Exception("Failed to create GLX context.");
    }

    return context;
}