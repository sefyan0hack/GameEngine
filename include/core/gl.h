#pragma once
#include <core/Global_H.hpp>
#include <core/Function.hpp>
#include <glcorearb.h> // need repl  with glext.h

#if defined(WINDOWS_PLT)
#include <windows.h>
using WindHandl = HWND;
using HDC_D     = HDC;
using GLCTX     = HGLRC;

[[maybe_unused]] inline static auto wglCreateContextAttribsARB = (GLCTX(WINAPI*)(HDC_D, GLCTX, const int*))(nullptr);
[[maybe_unused]] inline static auto wglGetExtensionsStringARB = (const char *(WINAPI*)(HDC_D))(nullptr);
[[maybe_unused]] inline static auto wglSwapIntervalEXT = (BOOL(APIENTRY*)(int))(nullptr);

#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>

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
    extern GLCTX glXCreateContext( HDC_D dpy, XVisualInfo *vis, GLCTX shareList, Bool direct );
    extern Bool  glXMakeCurrent( HDC_D dpy, GLXDrawable drawable, GLCTX ctx);
    extern void  glXDestroyContext( HDC_D dpy, GLCTX ctx );
    extern void* glXGetProcAddress(const GLubyte * procName);
    extern GLXFBConfig *glXChooseFBConfig( HDC_D dpy, int screen, const int *attribList, int *nitems );
    extern XVisualInfo *glXGetVisualFromFBConfig( HDC_D dpy, GLXFBConfig config );
    extern void glXSwapBuffers(	HDC_D dpy, GLXDrawable drawable);
    extern const char * glXQueryExtensionsString(HDC_D dpy, int screen);
    extern void glXCopyContext( HDC_D dpy, GLCTX src, GLCTX dst, unsigned long mask);
    extern int glXGetFBConfigAttrib( HDC_D dpy, GLXFBConfig config, int attribute, int * value);
    extern GLXFBConfig * glXGetFBConfigs( HDC_D dpy, int screen, int * nelements);
}
[[maybe_unused]] inline static auto glXCreateContextAttribsARB = (GLCTX(*)(HDC_D dpy, GLXFBConfig config, GLCTX share_context, Bool direct, const int *attrib_list))(nullptr);
#endif 

inline constexpr auto GL_ERR_to_string(GLenum glError) -> const char*
{
    switch (glError)
    {
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_CONTEXT_LOST: return "GL_CONTEXT_LOST";
        default: return "GL_UNKNOWN";
    }
}

extern "C"{
    extern GLenum glGetError();
}

#define GLFUNCS(X)\
X(PFNGLCLEARCOLORPROC, glClearColor);\
X(PFNGLVIEWPORTPROC, glViewport);\
X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);\
X(PFNGLGENBUFFERSPROC, glGenBuffers);\
X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);\
X(PFNGLBINDBUFFERPROC, glBindBuffer);\
X(PFNGLBUFFERDATAPROC, glBufferData);\
X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);\
X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);\
X(PFNGLCREATESHADERPROC, glCreateShader);\
X(PFNGLDELETESHADERPROC, glDeleteShader);\
X(PFNGLSHADERSOURCEPROC, glShaderSource);\
X(PFNGLCOMPILESHADERPROC, glCompileShader);\
X(PFNGLGETSHADERIVPROC, glGetShaderiv);\
X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);\
X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced);\
X(PFNGLDRAWARRAYSPROC, glDrawArrays);\
X(PFNGLPOLYGONMODEPROC, glPolygonMode);\
X(PFNGLENABLEPROC, glEnable);\
X(PFNGLGETINTEGERVPROC, glGetIntegerv);\
X(PFNGLGETSTRINGPROC, glGetString);\
X(PFNGLCLEARPROC, glClear);\
X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);\
X(PFNGLGENTEXTURESPROC, glGenTextures);\
X(PFNGLTEXPARAMETERIPROC, glTexParameteri);\
X(PFNGLPIXELSTOREIPROC, glPixelStorei);\
X(PFNGLTEXIMAGE2DPROC, glTexImage2D);\
X(PFNGLACTIVETEXTUREPROC, glActiveTexture);\
X(PFNGLBINDTEXTUREPROC, glBindTexture);\
X(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);\
X(PFNGLCREATEPROGRAMPROC, glCreateProgram);\
X(PFNGLATTACHSHADERPROC, glAttachShader);\
X(PFNGLUSEPROGRAMPROC, glUseProgram);\
X(PFNGLDELETEPROGRAMPROC, glDeleteProgram);\
X(PFNGLLINKPROGRAMPROC, glLinkProgram);\
X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);\
X(PFNGLGETPROGRAMIVPROC, glGetProgramiv);\
X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);\
X(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform);\
X(PFNGLUNIFORM1IPROC, glUniform1i);\
X(PFNGLUNIFORM2IPROC, glUniform2i);\
X(PFNGLUNIFORM3IPROC, glUniform3i);\
X(PFNGLUNIFORM4IPROC, glUniform4i);\
X(PFNGLUNIFORM1UIPROC, glUniform1ui);\
X(PFNGLUNIFORM2UIPROC, glUniform2ui);\
X(PFNGLUNIFORM3UIPROC, glUniform3ui);\
X(PFNGLUNIFORM4UIPROC, glUniform4ui);\
X(PFNGLUNIFORM1FPROC, glUniform1f);\
X(PFNGLUNIFORM2FPROC, glUniform2f);\
X(PFNGLUNIFORM3FPROC, glUniform3f);\
X(PFNGLUNIFORM4FPROC, glUniform4f);\
X(PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv);\
X(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv);\
X(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);\
X(PFNGLGETSTRINGIPROC, glGetStringi);\
X(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);\
X(PFNGLDEBUGMESSAGECONTROLPROC, glDebugMessageControl);\
X(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced);\
X(PFNGLDRAWELEMENTSPROC, glDrawElements);\
X(PFNGLSTENCILFUNCPROC, glStencilFunc);\
X(PFNGLSTENCILOPPROC, glStencilOp);\
X(PFNGLSTENCILMASKPROC, glStencilMask);\
X(PFNGLDEPTHMASKPROC, glDepthMask);\
X(PFNGLDEPTHFUNCPROC, glDepthFunc);\
X(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);\
X(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);\
X(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib);\
X(PFNGLGETATTACHEDSHADERSPROC, glGetAttachedShaders);\
X(PFNGLBLENDEQUATIONPROC, glBlendEquation);\
X(PFNGLBLENDFUNCPROC, glBlendFunc);\
X(PFNGLCULLFACEPROC, glCullFace);\
X(PFNGLUNIFORM3FVPROC, glUniform3fv);\
X(PFNGLDELETETEXTURESPROC, glDeleteTextures);\
X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);\
X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);\
X(PFNGLGETVERTEXATTRIBIVPROC, glGetVertexAttribiv);\
X(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv);\
X(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv);\
X(PFNGLCOPYBUFFERSUBDATAPROC, glCopyBufferSubData);\
X(PFNGLISPROGRAMPROC, glIsProgram);\
X(PFNGLISBUFFERPROC, glIsBuffer);\
X(PFNGLISVERTEXARRAYPROC, glIsVertexArray);\
X(PFNGLISTEXTUREPROC, glIsTexture);\
X(PFNGLISSHADERPROC, glIsShader);\
X(PFNGLFRONTFACEPROC, glFrontFace);\
X(PFNGLDISABLEPROC, glDisable);\
X(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);\
X(PFNGLDETACHSHADERPROC, glDetachShader);\
X(PFNGLVERTEXATTRIBFORMATPROC, glVertexAttribFormat);\
X(PFNGLVERTEXATTRIBBINDINGPROC, glVertexAttribBinding);\
X(PFNGLBINDVERTEXBUFFERPROC, glBindVertexBuffer);\
X(PFNGLISENABLEDPROC, glIsEnabled);\
X(PFNGLGETFLOATVPROC, glGetFloatv);\
X(PFNGLCLEARTEXIMAGEPROC, glClearTexImage);\
X(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D);\
X(PFNGLTEXSTORAGE2DPROC, glTexStorage2D);\
X(PFNGLUNIFORM2FVPROC, glUniform2fv);\

#ifdef DEBUG
#   define GLFUN(type, name)\
    inline Function<type> name;
#else
#   define GLFUN(type, name)\
    inline type name = Function<type>::default_;
#endif

GLFUNCS(GLFUN)
