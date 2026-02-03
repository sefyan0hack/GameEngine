#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

using H_WIN     = HWND;
using H_SRF     = HDC;
using H_DSP     = uint32_t;
using GL_CTX    = HGLRC;

namespace gl {
    [[maybe_unused]] inline PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB = nullptr;
    [[maybe_unused]] inline PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = nullptr;
    [[maybe_unused]] inline PFNWGLSWAPINTERVALEXTPROC SwapIntervalEXT = nullptr;

    template <class T>
    inline auto GetProcAddress(const char* name) -> T { return reinterpret_cast<T>(wglGetProcAddress(name)); }
    inline auto GetCurrentContext() -> GL_CTX { return wglGetCurrentContext(); }
    constexpr int32_t DepthBufferBits = 32;
}
