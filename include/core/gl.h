#pragma once
#define GL_VERSION_4_0 0
#define GL_VERSION_4_1 0
#define GL_VERSION_4_2 0
#define GL_VERSION_4_3 0
#define GL_VERSION_4_4 0
#define GL_VERSION_4_5 0
#define GL_VERSION_4_6 0
#include <glcorearb.h> // need repl  with glext.h
#include <core/Function.hpp>

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

#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>

using WindHandl = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;
using HDC_D     = const char*;
using GLCTX     = EMSCRIPTEN_WEBGL_CONTEXT_HANDLE;

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
        default: return "GL_UNKNOWN";
    }
}

inline PFNGLGETERRORPROC glGetError = nullptr;

#define GLFUNCS(X)\
X(PFNGLCLEARCOLORPROC, ClearColor);\
X(PFNGLVIEWPORTPROC, Viewport);\
X(PFNGLGENVERTEXARRAYSPROC, GenVertexArrays);\
X(PFNGLGENBUFFERSPROC, GenBuffers);\
X(PFNGLBINDVERTEXARRAYPROC, BindVertexArray);\
X(PFNGLBINDBUFFERPROC, BindBuffer);\
X(PFNGLBUFFERDATAPROC, BufferData);\
X(PFNGLENABLEVERTEXATTRIBARRAYPROC, EnableVertexAttribArray);\
X(PFNGLVERTEXATTRIBPOINTERPROC, VertexAttribPointer);\
X(PFNGLCREATESHADERPROC, CreateShader);\
X(PFNGLDELETESHADERPROC, DeleteShader);\
X(PFNGLSHADERSOURCEPROC, ShaderSource);\
X(PFNGLCOMPILESHADERPROC, CompileShader);\
X(PFNGLGETSHADERIVPROC, GetShaderiv);\
X(PFNGLGETSHADERINFOLOGPROC, GetShaderInfoLog);\
X(PFNGLDRAWARRAYSINSTANCEDPROC, DrawArraysInstanced);\
X(PFNGLDRAWARRAYSPROC, DrawArrays);\
X(PFNGLENABLEPROC, Enable);\
X(PFNGLGETINTEGERVPROC, GetIntegerv);\
X(PFNGLGETSTRINGPROC, GetString);\
X(PFNGLCLEARPROC, Clear);\
X(PFNGLVERTEXATTRIBDIVISORPROC, VertexAttribDivisor);\
X(PFNGLGENTEXTURESPROC, GenTextures);\
X(PFNGLTEXPARAMETERIPROC, TexParameteri);\
X(PFNGLPIXELSTOREIPROC, PixelStorei);\
X(PFNGLTEXIMAGE2DPROC, TexImage2D);\
X(PFNGLACTIVETEXTUREPROC, ActiveTexture);\
X(PFNGLBINDTEXTUREPROC, BindTexture);\
X(PFNGLGENERATEMIPMAPPROC, GenerateMipmap);\
X(PFNGLCREATEPROGRAMPROC, CreateProgram);\
X(PFNGLATTACHSHADERPROC, AttachShader);\
X(PFNGLUSEPROGRAMPROC, UseProgram);\
X(PFNGLDELETEPROGRAMPROC, DeleteProgram);\
X(PFNGLLINKPROGRAMPROC, LinkProgram);\
X(PFNGLGETUNIFORMLOCATIONPROC, GetUniformLocation);\
X(PFNGLGETPROGRAMIVPROC, GetProgramiv);\
X(PFNGLGETPROGRAMINFOLOGPROC, GetProgramInfoLog);\
X(PFNGLGETACTIVEUNIFORMPROC, GetActiveUniform);\
X(PFNGLUNIFORM1IPROC, Uniform1i);\
X(PFNGLUNIFORM2IPROC, Uniform2i);\
X(PFNGLUNIFORM3IPROC, Uniform3i);\
X(PFNGLUNIFORM4IPROC, Uniform4i);\
X(PFNGLUNIFORM1UIPROC, Uniform1ui);\
X(PFNGLUNIFORM2UIPROC, Uniform2ui);\
X(PFNGLUNIFORM3UIPROC, Uniform3ui);\
X(PFNGLUNIFORM4UIPROC, Uniform4ui);\
X(PFNGLUNIFORM1FPROC, Uniform1f);\
X(PFNGLUNIFORM2FPROC, Uniform2f);\
X(PFNGLUNIFORM3FPROC, Uniform3f);\
X(PFNGLUNIFORM4FPROC, Uniform4f);\
X(PFNGLUNIFORMMATRIX2FVPROC, UniformMatrix2fv);\
X(PFNGLUNIFORMMATRIX3FVPROC, UniformMatrix3fv);\
X(PFNGLUNIFORMMATRIX4FVPROC, UniformMatrix4fv);\
X(PFNGLGETSTRINGIPROC, GetStringi);\
X(PFNGLDRAWELEMENTSINSTANCEDPROC, DrawElementsInstanced);\
X(PFNGLDRAWELEMENTSPROC, DrawElements);\
X(PFNGLSTENCILFUNCPROC, StencilFunc);\
X(PFNGLSTENCILOPPROC, StencilOp);\
X(PFNGLSTENCILMASKPROC, StencilMask);\
X(PFNGLDEPTHMASKPROC, DepthMask);\
X(PFNGLDEPTHFUNCPROC, DepthFunc);\
X(PFNGLDISABLEVERTEXATTRIBARRAYPROC, DisableVertexAttribArray);\
X(PFNGLGETATTRIBLOCATIONPROC, GetAttribLocation);\
X(PFNGLGETACTIVEATTRIBPROC, GetActiveAttrib);\
X(PFNGLGETATTACHEDSHADERSPROC, GetAttachedShaders);\
X(PFNGLBLENDEQUATIONPROC, BlendEquation);\
X(PFNGLBLENDFUNCPROC, BlendFunc);\
X(PFNGLCULLFACEPROC, CullFace);\
X(PFNGLUNIFORM3FVPROC, Uniform3fv);\
X(PFNGLDELETETEXTURESPROC, DeleteTextures);\
X(PFNGLDELETEVERTEXARRAYSPROC, DeleteVertexArrays);\
X(PFNGLDELETEBUFFERSPROC, DeleteBuffers);\
X(PFNGLGETVERTEXATTRIBIVPROC, GetVertexAttribiv);\
X(PFNGLGETVERTEXATTRIBPOINTERVPROC, GetVertexAttribPointerv);\
X(PFNGLGETBUFFERPARAMETERIVPROC, GetBufferParameteriv);\
X(PFNGLCOPYBUFFERSUBDATAPROC, CopyBufferSubData);\
X(PFNGLISPROGRAMPROC, IsProgram);\
X(PFNGLISBUFFERPROC, IsBuffer);\
X(PFNGLISVERTEXARRAYPROC, IsVertexArray);\
X(PFNGLISTEXTUREPROC, IsTexture);\
X(PFNGLISSHADERPROC, IsShader);\
X(PFNGLFRONTFACEPROC, FrontFace);\
X(PFNGLDISABLEPROC, Disable);\
X(PFNGLVALIDATEPROGRAMPROC, ValidateProgram);\
X(PFNGLDETACHSHADERPROC, DetachShader);\
X(PFNGLISENABLEDPROC, IsEnabled);\
X(PFNGLGETFLOATVPROC, GetFloatv);\
X(PFNGLTEXSUBIMAGE2DPROC, TexSubImage2D);\
X(PFNGLUNIFORM2FVPROC, Uniform2fv);

// X(PFNGLDEBUGMESSAGECALLBACKPROC, DebugMessageCallback);
// X(PFNGLDEBUGMESSAGECONTROLPROC, DebugMessageControl);