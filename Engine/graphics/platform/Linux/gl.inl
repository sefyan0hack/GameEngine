#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <GL/glxext.h>

using H_WIN     = Window;
using H_SRF     = Window;
using H_DSP     = Display*;
using GL_CTX    = GLXContext;

namespace gl {

    [[maybe_unused]] inline PFNGLXCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = nullptr;
    [[maybe_unused]] inline PFNGLXSWAPINTERVALEXTPROC SwapIntervalEXT = nullptr;

    
    template <class T>
    inline auto GetProcAddress(const char* name) -> T { return reinterpret_cast<T>(glXGetProcAddress(reinterpret_cast<const GLubyte*>(name))); }
    inline auto GetCurrentContext() -> GL_CTX { return glXGetCurrentContext(); }
    constexpr int32_t DepthBufferBits = 32;
}