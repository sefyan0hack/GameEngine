#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Exception.hpp>
OpenGL::~OpenGL()
{
    if (m_Context != GL_CTX{}) {
        emscripten_webgl_destroy_context(m_Context);
    }
}

auto OpenGL::make_current()  -> bool
{
    return emscripten_webgl_make_context_current(m_Context) == EMSCRIPTEN_RESULT_SUCCESS;
}

auto OpenGL::find_config([[maybe_unused]] const CWindow& window) -> GL_CFG
{
    return 0; // config not used in web impl
}

auto OpenGL::create_context() -> GL_CTX
{
    auto surface = m_Window.surface();

    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);

    attrs.majorVersion = 2;
    attrs.minorVersion = 0;

    attrs.depth = 1;
    attrs.stencil = 1;

    if(MSAA == 0) attrs.antialias = 0;
    else attrs.antialias = 1;

    auto context = emscripten_webgl_create_context(surface, &attrs);

    if (context <= 0) {
        throw Exception("Failed to create WebGL context: GPU acceleration may be disabled.");
    }

    return context;
}

auto OpenGL::platform_extensions() const -> std::string
{
    auto exts = emscripten_webgl_get_supported_extensions();
    std::string exts_str = exts;
    std::free(exts);
    return exts_str;
}