#include <gl.hpp>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/native_window.h>

using H_WIN     = ANativeWindow*;
using H_SRF     = EGLSurface;
using H_DSP     = EGLDisplay;
using GL_CTX    = EGLContext;
using FUNC_T    = __eglMustCastToProperFunctionPointerType;

namespace gl {
    inline auto GetProcAddress(const char* name) -> FUNC_T { return eglGetProcAddress(name); }
    inline auto GetCurrentContext() -> GL_CTX { return eglGetCurrentContext(); }
    constexpr int32_t DepthBufferBits = 16;
}