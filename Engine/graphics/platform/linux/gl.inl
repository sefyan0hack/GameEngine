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

#define XXXGetProcAddress(name) glXGetProcAddress((const GLubyte*)name)
#define XXXGetCurrentContext() glXGetCurrentContext()

[[maybe_unused]] inline PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = nullptr;
[[maybe_unused]] inline PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = nullptr;

namespace gl {
    constexpr int32_t DepthBufferBits = 32;
}