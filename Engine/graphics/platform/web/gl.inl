#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>
#include <GLES3/gl3.h>
#define GL_GLES_PROTOTYPES 1

using H_WIN     = const char*;
using H_SRF     = const char*;
using H_DSP     = uint32_t;
using GL_CTX    = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;

#define XXXGetProcAddress(name) emscripten_webgl_get_proc_address(name)
#define XXXGetCurrentContext() emscripten_webgl_get_current_context()

namespace gl {
    constexpr int32_t DepthBufferBits = 32;
}
