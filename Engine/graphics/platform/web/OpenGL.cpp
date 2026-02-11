#include "OpenGL.hpp"
#include "Window.hpp"

OpenGL::~OpenGL()
{
    if (m_Context != GL_CTX{}) {
        emscripten_webgl_destroy_context(m_Context);
    }
}

auto OpenGL::make_current_opengl()  -> bool
{
    return emscripten_webgl_make_context_current(m_Context) == EMSCRIPTEN_RESULT_SUCCESS;
}

auto OpenGL::create_opengl_context() -> GL_CTX
{
    auto surface = m_Window.surface();

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;
    attrs.minorVersion = 0;

    attrs.depth = 1;
    attrs.stencil = 1;
    attrs.antialias = 1;

    auto context = emscripten_webgl_create_context(surface, &attrs);

    if (context <= 0) {
        throw Exception("Failed to create WebGL context: GPU acceleration may be disabled.");
    }

    return context;
}

auto OpenGL::get_gl_extensions() const -> std::string_view
{
    auto ext = emscripten_webgl_get_supported_extensions();// TODO: free
    if(ext) return ext;
    else return {};
}