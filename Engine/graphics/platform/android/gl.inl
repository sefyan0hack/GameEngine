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

#define XXXGetProcAddress(name) eglGetProcAddress(name)
#define XXXGetCurrentContext() eglGetCurrentContext()

namespace gl {
    constexpr int32_t DepthBufferBits = 16;
}