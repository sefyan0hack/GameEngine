#pragma once


#include <core/Function.hpp>
#include <engine_export.h>
#define GL_GLEXT_PROTOTYPES
#include "gl.inl"

namespace gl {
    enum class API { CORE, ES };

    constexpr API api = 
    #if defined(CORE_GL)
        API::CORE;
    #elif defined(ES_GL)
        API::ES;
    #endif

    constexpr int32_t OPENGL_MAJOR_VERSION = 3;
    #if defined(CORE_GL)
        constexpr int32_t OPENGL_MINOR_VERSION = 3;
    #elif defined(ES_GL)
        constexpr int32_t OPENGL_MINOR_VERSION = 0;
    #endif

    constexpr int32_t StencilBufferBits = 8;
    constexpr int32_t ChannelBits       = 8;
    constexpr int32_t AlphaBits         = 8;

    inline decltype(&glClearColor) ClearColor;
    inline decltype(&glViewport) Viewport;
    inline decltype(&glGenVertexArrays) GenVertexArrays;
    inline decltype(&glGenBuffers) GenBuffers;
    inline decltype(&glBindVertexArray) BindVertexArray;
    inline decltype(&glBindBuffer) BindBuffer;
    inline decltype(&glBufferData) BufferData;
    inline decltype(&glEnableVertexAttribArray) EnableVertexAttribArray;
    inline decltype(&glVertexAttribPointer) VertexAttribPointer;
    inline decltype(&glCreateShader) CreateShader;
    inline decltype(&glDeleteShader) DeleteShader;
    inline decltype(&glShaderSource) ShaderSource;
    inline decltype(&glCompileShader) CompileShader;
    inline decltype(&glGetShaderiv) GetShaderiv;
    inline decltype(&glGetShaderInfoLog) GetShaderInfoLog;
    inline decltype(&glDrawArraysInstanced) DrawArraysInstanced;
    inline decltype(&glDrawArrays) DrawArrays;
    inline decltype(&glEnable) Enable;
    inline decltype(&glGetIntegerv) GetIntegerv;
    inline decltype(&glGetString) GetString;
    inline decltype(&glClear) Clear;
    inline decltype(&glVertexAttribDivisor) VertexAttribDivisor;
    inline decltype(&glGenTextures) GenTextures;
    inline decltype(&glTexParameteri) TexParameteri;
    inline decltype(&glPixelStorei) PixelStorei;
    inline decltype(&glTexImage2D) TexImage2D;
    inline decltype(&glActiveTexture) ActiveTexture;
    inline decltype(&glBindTexture) BindTexture;
    inline decltype(&glGenerateMipmap) GenerateMipmap;
    inline decltype(&glCreateProgram) CreateProgram;
    inline decltype(&glAttachShader) AttachShader;
    inline decltype(&glUseProgram) UseProgram;
    inline decltype(&glDeleteProgram) DeleteProgram;
    inline decltype(&glLinkProgram) LinkProgram;
    inline decltype(&glGetUniformLocation) GetUniformLocation;
    inline decltype(&glGetProgramiv) GetProgramiv;
    inline decltype(&glGetProgramInfoLog) GetProgramInfoLog;
    inline decltype(&glGetActiveUniform) GetActiveUniform;
    inline decltype(&glUniform1i) Uniform1i;
    inline decltype(&glUniform2i) Uniform2i;
    inline decltype(&glUniform3i) Uniform3i;
    inline decltype(&glUniform4i) Uniform4i;
    inline decltype(&glUniform1ui) Uniform1ui;
    inline decltype(&glUniform2ui) Uniform2ui;
    inline decltype(&glUniform3ui) Uniform3ui;
    inline decltype(&glUniform4ui) Uniform4ui;
    inline decltype(&glUniform1f) Uniform1f;
    inline decltype(&glUniform2f) Uniform2f;
    inline decltype(&glUniform3f) Uniform3f;
    inline decltype(&glUniform4f) Uniform4f;
    inline decltype(&glUniform2fv) Uniform2fv;
    inline decltype(&glUniform3fv) Uniform3fv;
    inline decltype(&glUniform4fv) Uniform4fv;
    inline decltype(&glUniformMatrix2fv) UniformMatrix2fv;
    inline decltype(&glUniformMatrix3fv) UniformMatrix3fv;
    inline decltype(&glUniformMatrix4fv) UniformMatrix4fv;
    inline decltype(&glGetStringi) GetStringi;
    inline decltype(&glDrawElementsInstanced) DrawElementsInstanced;
    inline decltype(&glDrawElements) DrawElements;
    inline decltype(&glStencilFunc) StencilFunc;
    inline decltype(&glStencilOp) StencilOp;
    inline decltype(&glStencilMask) StencilMask;
    inline decltype(&glDepthMask) DepthMask;
    inline decltype(&glDepthFunc) DepthFunc;
    inline decltype(&glDisableVertexAttribArray) DisableVertexAttribArray;
    inline decltype(&glGetAttribLocation) GetAttribLocation;
    inline decltype(&glGetActiveAttrib) GetActiveAttrib;
    inline decltype(&glGetAttachedShaders) GetAttachedShaders;
    inline decltype(&glBlendEquation) BlendEquation;
    inline decltype(&glBlendFunc) BlendFunc;
    inline decltype(&glCullFace) CullFace;
    inline decltype(&glDeleteTextures) DeleteTextures;
    inline decltype(&glDeleteVertexArrays) DeleteVertexArrays;
    inline decltype(&glDeleteBuffers) DeleteBuffers;
    inline decltype(&glGetVertexAttribiv) GetVertexAttribiv;
    inline decltype(&glGetVertexAttribPointerv) GetVertexAttribPointerv;
    inline decltype(&glGetBufferParameteriv) GetBufferParameteriv;
    inline decltype(&glCopyBufferSubData) CopyBufferSubData;
    inline decltype(&glIsProgram) IsProgram;
    inline decltype(&glIsBuffer) IsBuffer;
    inline decltype(&glIsVertexArray) IsVertexArray;
    inline decltype(&glIsTexture) IsTexture;
    inline decltype(&glIsShader) IsShader;
    inline decltype(&glFrontFace) FrontFace;
    inline decltype(&glDisable) Disable;
    inline decltype(&glValidateProgram) ValidateProgram;
    inline decltype(&glDetachShader) DetachShader;
    inline decltype(&glIsEnabled) IsEnabled;
    inline decltype(&glGetFloatv) GetFloatv;
    inline decltype(&glTexSubImage2D) TexSubImage2D;
    inline decltype(&glLineWidth) LineWidth;
    inline decltype(&glHint) Hint;
    inline decltype(&glPolygonOffset) PolygonOffset;
    inline decltype(&glBindBufferBase) BindBufferBase;
    inline decltype(&glBufferSubData) BufferSubData;
    inline decltype(&glGetUniformBlockIndex) GetUniformBlockIndex;
    inline decltype(&glUniformBlockBinding) UniformBlockBinding;
    inline decltype(&glGetBooleanv) GetBooleanv;
    inline decltype(&glFinish) Finish;

    //TODO: remove this and make some sort of extention system
#if defined(CORE_GL)
    inline decltype(&glPolygonMode) PolygonMode;
    inline decltype(&glPointSize) PointSize;
#endif

    ENGINE_EXPORT auto get_proc_address(const char* name) -> void*;
    ENGINE_EXPORT auto load_opengl_functions() -> void;
    ENGINE_EXPORT auto get_integer(GLenum name) -> GLint;
    ENGINE_EXPORT auto get_boolean(GLenum name) -> GLboolean;
} // namespace gl

#undef GL_GLEXT_PROTOTYPES