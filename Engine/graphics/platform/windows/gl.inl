#include <windows.h>
#include <GL/gl.h>
#include <glext.h>
#include <wglext.h>

using H_WIN     = HWND;
using H_SRF     = HDC;
using H_DSP     = uint32_t;
using GL_CTX    = HGLRC;
using FUNC_T    = PROC;

namespace gl {
    [[maybe_unused]] inline PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB = nullptr;
    [[maybe_unused]] inline PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = nullptr;
    [[maybe_unused]] inline PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT = nullptr;

    inline auto GetProcAddress(const char* name) -> FUNC_T { return wglGetProcAddress(name); }
    inline auto GetCurrentContext() -> GL_CTX { return wglGetCurrentContext(); }
    constexpr int32_t DepthBufferBits = 32;
}
