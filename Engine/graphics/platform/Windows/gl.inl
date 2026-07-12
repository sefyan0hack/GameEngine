#pragma once
#define WGL_WGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>
#include <GL/wglext.h>

using H_WIN     = HWND;
using H_SRF     = HDC;
using H_DSP     = int;
using GL_CTX    = HGLRC;
using GL_CFG    = int;

namespace gl {
    template <class T>
    inline auto GetProcAddress(const char* name) -> T {
        union {
            PROC proc;
            T func;
        } u{ wglGetProcAddress(name) };

        return u.func;
    }
    inline auto GetCurrentContext() -> GL_CTX { return wglGetCurrentContext(); }
}
