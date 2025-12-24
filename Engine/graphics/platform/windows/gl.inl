#include <windows.h>
#include <GL/gl.h>
#include <glext.h>
#include <wglext.h>

using H_WIN     = HWND;
using H_SRF     = HDC;
using H_DSP     = uint32_t;
using GL_CTX    = HGLRC;

#define XXXGetProcAddress(name) wglGetProcAddress(name)
#define XXXGetCurrentContext() wglGetCurrentContext()

// Declare function pointers using typedefs
[[maybe_unused]] inline PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
[[maybe_unused]] inline PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
[[maybe_unused]] inline PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

namespace gl {
    constexpr int32_t DepthBufferBits = 32;
}
