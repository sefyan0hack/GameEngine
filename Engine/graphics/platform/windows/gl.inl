#include <windows.h>
#include <GL/gl.h>
#include <glext.h>
#include <wglext.h>

using H_WIN     = HWND;
using H_SRF     = HDC;
using GL_CTX     = HGLRC;

#define XXXGetProcAddress(name) wglGetProcAddress(name)

// Declare function pointers using typedefs
[[maybe_unused]] inline static PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
[[maybe_unused]] inline static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
[[maybe_unused]] inline static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;


#define GLFUNCS GLFUNCS_COMMON\
    X(PolygonMode);\
    X(PointSize);\
    X(GetProgramResourceiv);\
    X(GetProgramResourceName);\
    X(GetProgramResourceLocation);\
    X(GetProgramInterfaceiv);
