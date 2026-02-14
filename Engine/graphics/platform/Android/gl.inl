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

namespace gl {
    template<class T>
    inline auto GetProcAddress(const char* name) -> T { return reinterpret_cast<T>(eglGetProcAddress(name)); }
    inline auto GetCurrentContext() -> GL_CTX { return eglGetCurrentContext(); }
    constexpr int32_t DepthBufferBits = 16;
}