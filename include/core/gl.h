#pragma once
#include <core/Function.hpp>

#define GL_GLEXT_PROTOTYPES
#if defined(WINDOWS_PLT)
#include <windows.h>
#include <GL/gl.h>
#include <glext.h>

using WindHandl = HWND;
using HDC_D     = HDC;
using GLCTX     = HGLRC;

#define XXXGetProcAddress(name) wglGetProcAddress(name)

[[maybe_unused]] inline static auto wglCreateContextAttribsARB = (GLCTX(WINAPI*)(HDC_D, GLCTX, const int*))(nullptr);
[[maybe_unused]] inline static auto wglGetExtensionsStringARB = (const char *(WINAPI*)(HDC_D))(nullptr);
[[maybe_unused]] inline static auto wglSwapIntervalEXT = (BOOL(APIENTRY*)(int))(nullptr);

#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <glext.h>

#define XXXGetProcAddress(name) glXGetProcAddress((const GLubyte*)name)

struct __GLXcontextRec;
using WindHandl = Window;
using HDC_D     = Display*;
using GLCTX     = __GLXcontextRec*;

[[maybe_unused]] inline static auto glXCreateContextAttribsARB = (GLCTX(*)(HDC_D dpy, GLXFBConfig config, GLCTX share_context, Bool direct, const int *attrib_list))(nullptr);

#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/key_codes.h>
#include <GL/gl.h>
#include "glext.h"

#define XXXGetProcAddress(name) emscripten_webgl_get_proc_address(name)

using WindHandl = const char*;
using HDC_D     = void*;
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

#define GLFUNCS_COMMON(X)\
    X(ClearColor);\
    X(Viewport);\
    X(GenVertexArrays);\
    X(GenBuffers);\
    X(BindVertexArray);\
    X(BindBuffer);\
    X(BufferData);\
    X(EnableVertexAttribArray);\
    X(VertexAttribPointer);\
    X(CreateShader);\
    X(DeleteShader);\
    X(ShaderSource);\
    X(CompileShader);\
    X(GetShaderiv);\
    X(GetShaderInfoLog);\
    X(DrawArraysInstanced);\
    X(DrawArrays);\
    X(Enable);\
    X(GetIntegerv);\
    X(GetString);\
    X(Clear);\
    X(VertexAttribDivisor);\
    X(GenTextures);\
    X(TexParameteri);\
    X(PixelStorei);\
    X(TexImage2D);\
    X(ActiveTexture);\
    X(BindTexture);\
    X(GenerateMipmap);\
    X(CreateProgram);\
    X(AttachShader);\
    X(UseProgram);\
    X(DeleteProgram);\
    X(LinkProgram);\
    X(GetUniformLocation);\
    X(GetProgramiv);\
    X(GetProgramInfoLog);\
    X(GetActiveUniform);\
    X(Uniform1i);\
    X(Uniform2i);\
    X(Uniform3i);\
    X(Uniform4i);\
    X(Uniform1ui);\
    X(Uniform2ui);\
    X(Uniform3ui);\
    X(Uniform4ui);\
    X(Uniform1f);\
    X(Uniform2f);\
    X(Uniform3f);\
    X(Uniform4f);\
    X(UniformMatrix2fv);\
    X(UniformMatrix3fv);\
    X(UniformMatrix4fv);\
    X(GetStringi);\
    X(DrawElementsInstanced);\
    X(DrawElements);\
    X(StencilFunc);\
    X(StencilOp);\
    X(StencilMask);\
    X(DepthMask);\
    X(DepthFunc);\
    X(DisableVertexAttribArray);\
    X(GetAttribLocation);\
    X(GetActiveAttrib);\
    X(GetAttachedShaders);\
    X(BlendEquation);\
    X(BlendFunc);\
    X(CullFace);\
    X(Uniform3fv);\
    X(DeleteTextures);\
    X(DeleteVertexArrays);\
    X(DeleteBuffers);\
    X(GetVertexAttribiv);\
    X(GetVertexAttribPointerv);\
    X(GetBufferParameteriv);\
    X(CopyBufferSubData);\
    X(IsProgram);\
    X(IsBuffer);\
    X(IsVertexArray);\
    X(IsTexture);\
    X(IsShader);\
    X(FrontFace);\
    X(Disable);\
    X(ValidateProgram);\
    X(DetachShader);\
    X(IsEnabled);\
    X(GetFloatv);\
    X(TexSubImage2D);\
    X(Uniform2fv);\
    X(PointSize);


#if defined(WEB_PLT)
  // web (or ES-only) build
  #define GLFUNCS(X)  GLFUNCS_COMMON(X)
#else
  // desktop GL
  #define GLFUNCS(X)\
      GLFUNCS_COMMON(X)\
      X(PolygonMode);\
      X(LineStipple);
#endif