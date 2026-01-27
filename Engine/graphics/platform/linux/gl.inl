#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glext.h>
#include <glxext.h>


using H_WIN     = Window;
using H_SRF     = Window;
using H_DSP     = Display*;
using GL_CTX    = GLXContext;
using FUNC_T    = void(*)();

[[maybe_unused]] inline PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = nullptr;
[[maybe_unused]] inline PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = nullptr;

namespace gl {
    inline auto GetProcAddress(const char* name) -> FUNC_T { return glXGetProcAddress(reinterpret_cast<const GLubyte*>(name)); }
    inline auto GetCurrentContext() -> GL_CTX { return glXGetCurrentContext(); }
    constexpr int32_t DepthBufferBits = 32;
}