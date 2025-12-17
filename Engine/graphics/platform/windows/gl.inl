#include <windows.h>
#include <GL/gl.h>
#include <glext.h>
#include <wglext.h>

using H_WIN     = HWND;
using H_SRF     = HDC;
using GL_CTX    = HGLRC;

#define XXXGetProcAddress(name) wglGetProcAddress(name)
#define XXXGetCurrentContext() wglGetCurrentContext()

// Declare function pointers using typedefs
[[maybe_unused]] inline PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
[[maybe_unused]] inline PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
[[maybe_unused]] inline PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;


#define GLFUNCS GLFUNCS_COMMON\
    X(PolygonMode)\
    X(PointSize)\
    X(GetProgramResourceiv)\
    X(GetProgramResourceName)\
    X(GetProgramResourceLocation)\
    X(GetProgramInterfaceiv)
