#include "OpenGL.hpp"
#include "Window.hpp"
#include <core/Log.hpp>
#include <cassert>

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
    for(int depth = 0; depth <= 1; ++depth)
    for(int stencil = 0; stencil <= 1; ++stencil)
    for(int antialias = 0; antialias <= 1; ++antialias)
    {
        emscripten_webgl_init_context_attributes(&attrs);

        attrs.depth = depth;
        attrs.stencil = stencil;
        attrs.antialias = antialias;

        debug::log("Requesting depth:{}, stencil: {}, antialias: {}", depth, stencil, antialias);

        auto context = emscripten_webgl_create_context(surface, &attrs);

        if (context <= 0) {
            throw Exception("Failed to create WebGL context: GPU acceleration may be disabled.");
        }

        return context;
    }
}