#pragma once
#define GL_GLES_PROTOTYPES 1
#define GL_GLEXT_PROTOTYPES 1
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>
#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>

using H_WIN     = const char*;
using H_SRF     = const char*;
using H_DSP     = int;
using GL_CTX    = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;
using GL_CFG    = int;

namespace gl {
    template <class T>
    inline auto GetProcAddress(const char* name) -> T { return reinterpret_cast<T>(emscripten_webgl_get_proc_address(name)); }
    inline auto GetCurrentContext() -> GL_CTX { return emscripten_webgl_get_current_context(); }
}