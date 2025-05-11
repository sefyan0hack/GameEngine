#pragma once

#include <core/gl.h>

#if defined(WINDOWS_PLT)
#include <windows.h>
using WindHandl = HWND;
using HDC_D     = HDC;
using GLCTX     = HGLRC;
auto __GetProcAddress(LPCSTR module, const char* name) -> void*;
auto rsgl(const char* name) -> void*;
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifndef GL_GLEXT_PROTOTYPES
#undef GL_GLEXT_PROTOTYPES
#endif
// #include <GL/glxext.h>
struct __GLXcontextRec;
using WindHandl = Window;
using HDC_D     = Display*;
using GLCTX     = __GLXcontextRec*;

typedef XID GLXPixmap;
typedef XID GLXDrawable;
/* GLX 1.3 and later */
typedef struct __GLXFBConfigRec *GLXFBConfig;
typedef XID GLXFBConfigID;
typedef XID GLXContextID;
typedef XID GLXWindow;
typedef XID GLXPbuffer;
extern "C" {
    extern GLCTX glXCreateContext( Display *dpy, XVisualInfo *vis, GLCTX shareList, Bool direct );
    extern Bool  glXMakeCurrent( Display *dpy, GLXDrawable drawable, GLCTX ctx);
    extern void  glXDestroyContext( Display *dpy, GLCTX ctx );
    extern void* glXGetProcAddress(const GLubyte * procName);
    extern GLXFBConfig *glXChooseFBConfig( Display *dpy, int screen, const int *attribList, int *nitems );
    extern XVisualInfo *glXGetVisualFromFBConfig( Display *dpy, GLXFBConfig config );
    extern void glXSwapBuffers(	Display * dpy, GLXDrawable drawable);   
    [[maybe_unused]] inline static auto glXCreateContextAttribsARB = (GLCTX(*)(Display *dpy, GLXFBConfig config, GLCTX share_context, Bool direct, const int *attrib_list))(nullptr);
}

#endif

class OpenGL
{
  FOR_TEST(OpenGL)
    public:
        explicit OpenGL(WindHandl window, HDC_D hdcd = HDC_D{});
        OpenGL(const OpenGL& other);
        OpenGL(OpenGL&& other) noexcept;
        ~OpenGL();
        auto operator = (const OpenGL& other) -> OpenGL;
        auto operator = (OpenGL&& other)  noexcept -> OpenGL;
        auto operator == (const OpenGL& other) const -> bool;
        auto operator != (const OpenGL& other) const -> bool;
        operator bool () const;

    public:
        auto DrawContext() const -> HDC_D;
        auto MajorV() const -> GLint;
        auto MinorV() const -> GLint;
        auto isValid() const -> bool;
        auto CreationTime() const -> std::time_t;
        auto isDebugable() const -> bool;

        static auto Vendor() -> std::string;
        static auto Renderer() -> std::string;
        static auto GlslVersions() -> std::vector<std::string>;
        static auto Extensions() -> std::vector<std::string>;
        static auto MaxTextureUnits() -> GLint;

    private:
        #if defined(WINDOWS_PLT)
        friend auto __GetProcAddress(LPCSTR module, const char* name) -> void*;
        friend auto rsgl(const char* name) -> void*;
        inline static LPCSTR OPENGL_MODULE_NAME {OPENGL_LIB};    
        auto init_opengl_win32()              -> void ;
        #elif defined(LINUX_PLT)
        auto init_opengl_linux(Window window) -> void ;
        #endif //_WIN32

    private:
        GLCTX m_Context;
        HDC_D m_MainHDC;
        GLint m_Major;
        GLint m_Minor;
        std::time_t m_CreationTime;
        bool m_Debug;

        inline static std::string m_Vendor;
        inline static std::string m_Renderer;
        inline static std::vector<std::string> m_GlslVersions;
        inline static std::vector<std::string> m_Extensions;
        inline static GLint m_MaxTextureUnits;

        enum {
            #if defined(WINDOWS_PLT)
            WGL_DRAW_TO_WINDOW_ARB           = 0x2001,
            WGL_ACCELERATION_ARB             = 0x2003,
            WGL_SUPPORT_OPENGL_ARB           = 0x2010,
            WGL_DOUBLE_BUFFER_ARB            = 0x2011,
            WGL_PIXEL_TYPE_ARB               = 0x2013,
            WGL_COLOR_BITS_ARB               = 0x2014,
            WGL_DEPTH_BITS_ARB               = 0x2022,
            WGL_STENCIL_BITS_ARB             = 0x2023,
            WGL_FULL_ACCELERATION_ARB        = 0x2027,
            WGL_TYPE_RGBA_ARB                = 0x202B,
            WGL_CONTEXT_MAJOR_VERSION_ARB    = 0x2091,
            WGL_CONTEXT_MINOR_VERSION_ARB    = 0x2092,
            WGL_CONTEXT_FLAGS_ARB            = 0x2094,
            WGL_CONTEXT_PROFILE_MASK_ARB     = 0x9126,
            ERROR_INVALID_VERSION_ARB        = 0x2095,
            ERROR_INVALID_PROFILE_ARB        = 0x2096,
            WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB = 0x8256,
            GL_ALL_ATTRIB_BITS               = 0x000fffff,
            #elif defined(LINUX_PLT)
            GLX_NONE        			    = 0x8000,
            GLX_DRAWABLE_TYPE               = 0x8010,
            GLX_RENDER_TYPE                 = 0x8011,
            GLX_X_RENDERABLE                = 0x8012,
            GLX_DOUBLEBUFFER                = 5,
            GLX_RED_SIZE		            = 8,
            GLX_GREEN_SIZE		            = 9,
            GLX_BLUE_SIZE		            = 10,
            GLX_ALPHA_SIZE		            = 11,
            GLX_DEPTH_SIZE		            = 12,
            #endif 
        };
};