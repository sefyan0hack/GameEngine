#pragma once
#define GL_GLEXT_PROTOTYPES

#include <engine_export.h>
#include "gl.inl"

#define GET_GLEXT_FUNCTION_THROW(func)\
static auto func##_ext = [](){\
    auto r = gl::GetProcAddress<decltype(&func)>(#func);\
    if (r) return r;\
    else throw Exception("Failed to load "#func". (maybe not supported)");\
}()

#define GET_GLEXT_FUNCTION_NO_THROW(func) static auto func##_ext = gl::GetProcAddress<decltype(&func)>(#func)

#define FUNCTIONS_GL_LIST\
    FUNC_GL_X(ClearColor)\
    FUNC_GL_X(Viewport)\
    FUNC_GL_X(GenVertexArrays)\
    FUNC_GL_X(GenBuffers)\
    FUNC_GL_X(BindVertexArray)\
    FUNC_GL_X(BindBuffer)\
    FUNC_GL_X(BufferData)\
    FUNC_GL_X(EnableVertexAttribArray)\
    FUNC_GL_X(VertexAttribPointer)\
    FUNC_GL_X(CreateShader)\
    FUNC_GL_X(DeleteShader)\
    FUNC_GL_X(ShaderSource)\
    FUNC_GL_X(CompileShader)\
    FUNC_GL_X(GetShaderiv)\
    FUNC_GL_X(GetShaderInfoLog)\
    FUNC_GL_X(DrawArraysInstanced)\
    FUNC_GL_X(DrawArrays)\
    FUNC_GL_X(Enable)\
    FUNC_GL_X(GetIntegerv)\
    FUNC_GL_X(GetString)\
    FUNC_GL_X(Clear)\
    FUNC_GL_X(VertexAttribDivisor)\
    FUNC_GL_X(GenTextures)\
    FUNC_GL_X(TexParameteri)\
    FUNC_GL_X(PixelStorei)\
    FUNC_GL_X(TexImage2D)\
    FUNC_GL_X(ActiveTexture)\
    FUNC_GL_X(BindTexture)\
    FUNC_GL_X(GenerateMipmap)\
    FUNC_GL_X(CreateProgram)\
    FUNC_GL_X(AttachShader)\
    FUNC_GL_X(UseProgram)\
    FUNC_GL_X(DeleteProgram)\
    FUNC_GL_X(LinkProgram)\
    FUNC_GL_X(GetUniformLocation)\
    FUNC_GL_X(GetProgramiv)\
    FUNC_GL_X(GetProgramInfoLog)\
    FUNC_GL_X(GetActiveUniform)\
    FUNC_GL_X(Uniform1i)\
    FUNC_GL_X(Uniform2i)\
    FUNC_GL_X(Uniform3i)\
    FUNC_GL_X(Uniform4i)\
    FUNC_GL_X(Uniform1ui)\
    FUNC_GL_X(Uniform2ui)\
    FUNC_GL_X(Uniform3ui)\
    FUNC_GL_X(Uniform4ui)\
    FUNC_GL_X(Uniform1f)\
    FUNC_GL_X(Uniform2f)\
    FUNC_GL_X(Uniform3f)\
    FUNC_GL_X(Uniform4f)\
    FUNC_GL_X(Uniform2fv)\
    FUNC_GL_X(Uniform3fv)\
    FUNC_GL_X(Uniform4fv)\
    FUNC_GL_X(Uniform2iv)\
    FUNC_GL_X(Uniform3iv)\
    FUNC_GL_X(Uniform4iv)\
    FUNC_GL_X(Uniform2uiv)\
    FUNC_GL_X(Uniform3uiv)\
    FUNC_GL_X(Uniform4uiv)\
    FUNC_GL_X(UniformMatrix2fv)\
    FUNC_GL_X(UniformMatrix3fv)\
    FUNC_GL_X(UniformMatrix4fv)\
    FUNC_GL_X(GetStringi)\
    FUNC_GL_X(DrawElementsInstanced)\
    FUNC_GL_X(DrawElements)\
    FUNC_GL_X(StencilFunc)\
    FUNC_GL_X(StencilOp)\
    FUNC_GL_X(StencilMask)\
    FUNC_GL_X(DepthMask)\
    FUNC_GL_X(DepthFunc)\
    FUNC_GL_X(DisableVertexAttribArray)\
    FUNC_GL_X(GetAttribLocation)\
    FUNC_GL_X(GetActiveAttrib)\
    FUNC_GL_X(GetAttachedShaders)\
    FUNC_GL_X(BlendEquation)\
    FUNC_GL_X(BlendFunc)\
    FUNC_GL_X(CullFace)\
    FUNC_GL_X(DeleteTextures)\
    FUNC_GL_X(DeleteVertexArrays)\
    FUNC_GL_X(DeleteBuffers)\
    FUNC_GL_X(GetVertexAttribiv)\
    FUNC_GL_X(GetVertexAttribPointerv)\
    FUNC_GL_X(GetBufferParameteriv)\
    FUNC_GL_X(CopyBufferSubData)\
    FUNC_GL_X(IsProgram)\
    FUNC_GL_X(IsBuffer)\
    FUNC_GL_X(IsVertexArray)\
    FUNC_GL_X(IsTexture)\
    FUNC_GL_X(IsShader)\
    FUNC_GL_X(FrontFace)\
    FUNC_GL_X(Disable)\
    FUNC_GL_X(ValidateProgram)\
    FUNC_GL_X(DetachShader)\
    FUNC_GL_X(IsEnabled)\
    FUNC_GL_X(GetFloatv)\
    FUNC_GL_X(TexSubImage2D)\
    FUNC_GL_X(LineWidth)\
    FUNC_GL_X(Hint)\
    FUNC_GL_X(PolygonOffset)\
    FUNC_GL_X(BindBufferBase)\
    FUNC_GL_X(BufferSubData)\
    FUNC_GL_X(GetUniformBlockIndex)\
    FUNC_GL_X(UniformBlockBinding)\
    FUNC_GL_X(GetBooleanv)\
    FUNC_GL_X(VertexAttribIPointer)\
    FUNC_GL_X(Finish)\
    FUNC_GL_X(GenFramebuffers)\
    FUNC_GL_X(BindFramebuffer)\
    FUNC_GL_X(FramebufferTexture2D)\
    FUNC_GL_X(GenRenderbuffers)\
    FUNC_GL_X(BindRenderbuffer)\
    FUNC_GL_X(RenderbufferStorage)\
    FUNC_GL_X(FramebufferRenderbuffer)\
    FUNC_GL_X(CheckFramebufferStatus)\
    FUNC_GL_X(DrawBuffers)\
    FUNC_GL_X(ReadBuffer)\
    FUNC_GL_X(GetBooleani_v)\
    FUNC_GL_X(GetIntegeri_v)

namespace gl {

#define FUNC_GL_X(name) inline decltype(&gl##name) name;
    FUNCTIONS_GL_LIST
#undef FUNC_GL_X

} // namespace gl
