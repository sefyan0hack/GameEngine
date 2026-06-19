#define EGL_EGLEXT_PROTOTYPES
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
using GL_CFG    = EGLConfig;

namespace gl {
    template<class T>
    inline auto GetProcAddress(const char* name) -> T { return reinterpret_cast<T>(eglGetProcAddress(name)); }
    inline auto GetCurrentContext() -> GL_CTX { return eglGetCurrentContext(); }
}