#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glext.h>
#include <glxext.h>

#define XXXGetProcAddress(name) glXGetProcAddress((const GLubyte*)name)

struct __GLXcontextRec;
using H_WIN     = Window;
using H_SRF     = Display*;
using GLCTX     = __GLXcontextRec*;

[[maybe_unused]] inline static auto glXCreateContextAttribsARB = (GLCTX(*)(H_SRF dpy, GLXFBConfig config, GLCTX share_context, Bool direct, const int *attrib_list))(nullptr);
[[maybe_unused]] inline static PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT = nullptr;


#define GLFUNCS GLFUNCS_COMMON\
    X(PolygonMode);\
    X(PointSize);\
    X(GetProgramResourceiv);\
    X(GetProgramResourceName);\
    X(GetProgramResourceLocation);\
    X(GetProgramInterfaceiv);
