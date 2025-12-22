#include <gl.hpp>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <android/native_window.h>
#include <android_native_app_glue.h>

using H_WIN     = ANativeWindow;
using H_SRF     = EGLSurface;
using H_DSP     = EGLDisplay;
using GL_CTX    = EGLContext;


#define GLFUNCS GLFUNCS_COMMON
