#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>
#include <GLES3/gl3.h>

using H_WIN     = const char*;
using H_SRF     = const char*;
using H_DSP     = uint32_t;
using GL_CTX    = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;

namespace gl {
    template <class T>
    inline auto GetProcAddress(const char* name) -> T { return reinterpret_cast<T>(emscripten_webgl_get_proc_address(name)); }
    inline auto GetCurrentContext() -> GL_CTX { return emscripten_webgl_get_current_context(); }
    constexpr int32_t DepthBufferBits = 32;
}
