#pragma once

#define GLFUNCS_COMMON\
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
    X(Uniform2fv);\
    X(Uniform3fv);\
    X(Uniform4fv);\
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
    X(LineWidth);\
    X(Hint);\
    X(PolygonOffset);\
    X(BindBufferBase);\
    X(BufferSubData);\
    X(GetUniformBlockIndex);\
    X(UniformBlockBinding);\
    X(GetBooleanv);\
    X(Finish);


#define GL_GLEXT_PROTOTYPES

#if defined(WINDOWS_PLT)
#include "platform/win32/gl.inl"
#elif defined(LINUX_PLT)
#include "platform/linux/gl.inl"
#elif defined(WEB_PLT)
#include "platform/web/gl.inl"
#endif
