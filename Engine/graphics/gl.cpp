#include "gl.hpp"
#include <core/Log.hpp>
#include <core/SysInfo.hpp>

namespace gl {
    auto get_proc_address(const char* name) -> void* {
        void *address = gl::GetProcAddress<void*>(name);

        if(address == nullptr){
            address = os::get_proc_address(EG_OPENGL_MODULE_NAME, name);
        }

        if (address == nullptr) {
            throw Exception("Couldn't load {} function `{}`", EG_OPENGL_MODULE_NAME, name);
        }

        return address;
    }

    auto load_opengl_functions() -> void
    {
        gl::ClearColor = reinterpret_cast<decltype(&glClearColor)>(gl::get_proc_address("glClearColor"));
        gl::Viewport = reinterpret_cast<decltype(&glViewport)>(gl::get_proc_address("glViewport"));
        gl::GenVertexArrays = reinterpret_cast<decltype(&glGenVertexArrays)>(gl::get_proc_address("glGenVertexArrays"));
        gl::GenBuffers = reinterpret_cast<decltype(&glGenBuffers)>(gl::get_proc_address("glGenBuffers"));
        gl::BindVertexArray = reinterpret_cast<decltype(&glBindVertexArray)>(gl::get_proc_address("glBindVertexArray"));
        gl::BindBuffer = reinterpret_cast<decltype(&glBindBuffer)>(gl::get_proc_address("glBindBuffer"));
        gl::BufferData = reinterpret_cast<decltype(&glBufferData)>(gl::get_proc_address("glBufferData"));
        gl::EnableVertexAttribArray = reinterpret_cast<decltype(&glEnableVertexAttribArray)>(gl::get_proc_address("glEnableVertexAttribArray"));
        gl::VertexAttribPointer = reinterpret_cast<decltype(&glVertexAttribPointer)>(gl::get_proc_address("glVertexAttribPointer"));
        gl::CreateShader = reinterpret_cast<decltype(&glCreateShader)>(gl::get_proc_address("glCreateShader"));
        gl::DeleteShader = reinterpret_cast<decltype(&glDeleteShader)>(gl::get_proc_address("glDeleteShader"));
        gl::ShaderSource = reinterpret_cast<decltype(&glShaderSource)>(gl::get_proc_address("glShaderSource"));
        gl::CompileShader = reinterpret_cast<decltype(&glCompileShader)>(gl::get_proc_address("glCompileShader"));
        gl::GetShaderiv = reinterpret_cast<decltype(&glGetShaderiv)>(gl::get_proc_address("glGetShaderiv"));
        gl::GetShaderInfoLog = reinterpret_cast<decltype(&glGetShaderInfoLog)>(gl::get_proc_address("glGetShaderInfoLog"));
        gl::DrawArraysInstanced = reinterpret_cast<decltype(&glDrawArraysInstanced)>(gl::get_proc_address("glDrawArraysInstanced"));
        gl::DrawArrays = reinterpret_cast<decltype(&glDrawArrays)>(gl::get_proc_address("glDrawArrays"));
        gl::Enable = reinterpret_cast<decltype(&glEnable)>(gl::get_proc_address("glEnable"));
        gl::GetIntegerv = reinterpret_cast<decltype(&glGetIntegerv)>(gl::get_proc_address("glGetIntegerv"));
        gl::GetString = reinterpret_cast<decltype(&glGetString)>(gl::get_proc_address("glGetString"));
        gl::Clear = reinterpret_cast<decltype(&glClear)>(gl::get_proc_address("glClear"));
        gl::VertexAttribDivisor = reinterpret_cast<decltype(&glVertexAttribDivisor)>(gl::get_proc_address("glVertexAttribDivisor"));
        gl::GenTextures = reinterpret_cast<decltype(&glGenTextures)>(gl::get_proc_address("glGenTextures"));
        gl::TexParameteri = reinterpret_cast<decltype(&glTexParameteri)>(gl::get_proc_address("glTexParameteri"));
        gl::PixelStorei = reinterpret_cast<decltype(&glPixelStorei)>(gl::get_proc_address("glPixelStorei"));
        gl::TexImage2D = reinterpret_cast<decltype(&glTexImage2D)>(gl::get_proc_address("glTexImage2D"));
        gl::ActiveTexture = reinterpret_cast<decltype(&glActiveTexture)>(gl::get_proc_address("glActiveTexture"));
        gl::BindTexture = reinterpret_cast<decltype(&glBindTexture)>(gl::get_proc_address("glBindTexture"));
        gl::GenerateMipmap = reinterpret_cast<decltype(&glGenerateMipmap)>(gl::get_proc_address("glGenerateMipmap"));
        gl::CreateProgram = reinterpret_cast<decltype(&glCreateProgram)>(gl::get_proc_address("glCreateProgram"));
        gl::AttachShader = reinterpret_cast<decltype(&glAttachShader)>(gl::get_proc_address("glAttachShader"));
        gl::UseProgram = reinterpret_cast<decltype(&glUseProgram)>(gl::get_proc_address("glUseProgram"));
        gl::DeleteProgram = reinterpret_cast<decltype(&glDeleteProgram)>(gl::get_proc_address("glDeleteProgram"));
        gl::LinkProgram = reinterpret_cast<decltype(&glLinkProgram)>(gl::get_proc_address("glLinkProgram"));
        gl::GetUniformLocation = reinterpret_cast<decltype(&glGetUniformLocation)>(gl::get_proc_address("glGetUniformLocation"));
        gl::GetProgramiv = reinterpret_cast<decltype(&glGetProgramiv)>(gl::get_proc_address("glGetProgramiv"));
        gl::GetProgramInfoLog = reinterpret_cast<decltype(&glGetProgramInfoLog)>(gl::get_proc_address("glGetProgramInfoLog"));
        gl::GetActiveUniform = reinterpret_cast<decltype(&glGetActiveUniform)>(gl::get_proc_address("glGetActiveUniform"));
        gl::Uniform1i = reinterpret_cast<decltype(&glUniform1i)>(gl::get_proc_address("glUniform1i"));
        gl::Uniform2i = reinterpret_cast<decltype(&glUniform2i)>(gl::get_proc_address("glUniform2i"));
        gl::Uniform3i = reinterpret_cast<decltype(&glUniform3i)>(gl::get_proc_address("glUniform3i"));
        gl::Uniform4i = reinterpret_cast<decltype(&glUniform4i)>(gl::get_proc_address("glUniform4i"));
        gl::Uniform1ui = reinterpret_cast<decltype(&glUniform1ui)>(gl::get_proc_address("glUniform1ui"));
        gl::Uniform2ui = reinterpret_cast<decltype(&glUniform2ui)>(gl::get_proc_address("glUniform2ui"));
        gl::Uniform3ui = reinterpret_cast<decltype(&glUniform3ui)>(gl::get_proc_address("glUniform3ui"));
        gl::Uniform4ui = reinterpret_cast<decltype(&glUniform4ui)>(gl::get_proc_address("glUniform4ui"));
        gl::Uniform1f = reinterpret_cast<decltype(&glUniform1f)>(gl::get_proc_address("glUniform1f"));
        gl::Uniform2f = reinterpret_cast<decltype(&glUniform2f)>(gl::get_proc_address("glUniform2f"));
        gl::Uniform3f = reinterpret_cast<decltype(&glUniform3f)>(gl::get_proc_address("glUniform3f"));
        gl::Uniform4f = reinterpret_cast<decltype(&glUniform4f)>(gl::get_proc_address("glUniform4f"));
        gl::Uniform2fv = reinterpret_cast<decltype(&glUniform2fv)>(gl::get_proc_address("glUniform2fv"));
        gl::Uniform3fv = reinterpret_cast<decltype(&glUniform3fv)>(gl::get_proc_address("glUniform3fv"));
        gl::Uniform4fv = reinterpret_cast<decltype(&glUniform4fv)>(gl::get_proc_address("glUniform4fv"));
        gl::UniformMatrix2fv = reinterpret_cast<decltype(&glUniformMatrix2fv)>(gl::get_proc_address("glUniformMatrix2fv"));
        gl::UniformMatrix3fv = reinterpret_cast<decltype(&glUniformMatrix3fv)>(gl::get_proc_address("glUniformMatrix3fv"));
        gl::UniformMatrix4fv = reinterpret_cast<decltype(&glUniformMatrix4fv)>(gl::get_proc_address("glUniformMatrix4fv"));
        gl::GetStringi = reinterpret_cast<decltype(&glGetStringi)>(gl::get_proc_address("glGetStringi"));
        gl::DrawElementsInstanced = reinterpret_cast<decltype(&glDrawElementsInstanced)>(gl::get_proc_address("glDrawElementsInstanced"));
        gl::DrawElements = reinterpret_cast<decltype(&glDrawElements)>(gl::get_proc_address("glDrawElements"));
        gl::StencilFunc = reinterpret_cast<decltype(&glStencilFunc)>(gl::get_proc_address("glStencilFunc"));
        gl::StencilOp = reinterpret_cast<decltype(&glStencilOp)>(gl::get_proc_address("glStencilOp"));
        gl::StencilMask = reinterpret_cast<decltype(&glStencilMask)>(gl::get_proc_address("glStencilMask"));
        gl::DepthMask = reinterpret_cast<decltype(&glDepthMask)>(gl::get_proc_address("glDepthMask"));
        gl::DepthFunc = reinterpret_cast<decltype(&glDepthFunc)>(gl::get_proc_address("glDepthFunc"));
        gl::DisableVertexAttribArray = reinterpret_cast<decltype(&glDisableVertexAttribArray)>(gl::get_proc_address("glDisableVertexAttribArray"));
        gl::GetAttribLocation = reinterpret_cast<decltype(&glGetAttribLocation)>(gl::get_proc_address("glGetAttribLocation"));
        gl::GetActiveAttrib = reinterpret_cast<decltype(&glGetActiveAttrib)>(gl::get_proc_address("glGetActiveAttrib"));
        gl::GetAttachedShaders = reinterpret_cast<decltype(&glGetAttachedShaders)>(gl::get_proc_address("glGetAttachedShaders"));
        gl::BlendEquation = reinterpret_cast<decltype(&glBlendEquation)>(gl::get_proc_address("glBlendEquation"));
        gl::BlendFunc = reinterpret_cast<decltype(&glBlendFunc)>(gl::get_proc_address("glBlendFunc"));
        gl::CullFace = reinterpret_cast<decltype(&glCullFace)>(gl::get_proc_address("glCullFace"));
        gl::DeleteTextures = reinterpret_cast<decltype(&glDeleteTextures)>(gl::get_proc_address("glDeleteTextures"));
        gl::DeleteVertexArrays = reinterpret_cast<decltype(&glDeleteVertexArrays)>(gl::get_proc_address("glDeleteVertexArrays"));
        gl::DeleteBuffers = reinterpret_cast<decltype(&glDeleteBuffers)>(gl::get_proc_address("glDeleteBuffers"));
        gl::GetVertexAttribiv = reinterpret_cast<decltype(&glGetVertexAttribiv)>(gl::get_proc_address("glGetVertexAttribiv"));
        gl::GetVertexAttribPointerv = reinterpret_cast<decltype(&glGetVertexAttribPointerv)>(gl::get_proc_address("glGetVertexAttribPointerv"));
        gl::GetBufferParameteriv = reinterpret_cast<decltype(&glGetBufferParameteriv)>(gl::get_proc_address("glGetBufferParameteriv"));
        gl::CopyBufferSubData = reinterpret_cast<decltype(&glCopyBufferSubData)>(gl::get_proc_address("glCopyBufferSubData"));
        gl::IsProgram = reinterpret_cast<decltype(&glIsProgram)>(gl::get_proc_address("glIsProgram"));
        gl::IsBuffer = reinterpret_cast<decltype(&glIsBuffer)>(gl::get_proc_address("glIsBuffer"));
        gl::IsVertexArray = reinterpret_cast<decltype(&glIsVertexArray)>(gl::get_proc_address("glIsVertexArray"));
        gl::IsTexture = reinterpret_cast<decltype(&glIsTexture)>(gl::get_proc_address("glIsTexture"));
        gl::IsShader = reinterpret_cast<decltype(&glIsShader)>(gl::get_proc_address("glIsShader"));
        gl::FrontFace = reinterpret_cast<decltype(&glFrontFace)>(gl::get_proc_address("glFrontFace"));
        gl::Disable = reinterpret_cast<decltype(&glDisable)>(gl::get_proc_address("glDisable"));
        gl::ValidateProgram = reinterpret_cast<decltype(&glValidateProgram)>(gl::get_proc_address("glValidateProgram"));
        gl::DetachShader = reinterpret_cast<decltype(&glDetachShader)>(gl::get_proc_address("glDetachShader"));
        gl::IsEnabled = reinterpret_cast<decltype(&glIsEnabled)>(gl::get_proc_address("glIsEnabled"));
        gl::GetFloatv = reinterpret_cast<decltype(&glGetFloatv)>(gl::get_proc_address("glGetFloatv"));
        gl::TexSubImage2D = reinterpret_cast<decltype(&glTexSubImage2D)>(gl::get_proc_address("glTexSubImage2D"));
        gl::LineWidth = reinterpret_cast<decltype(&glLineWidth)>(gl::get_proc_address("glLineWidth"));
        gl::Hint = reinterpret_cast<decltype(&glHint)>(gl::get_proc_address("glHint"));
        gl::PolygonOffset = reinterpret_cast<decltype(&glPolygonOffset)>(gl::get_proc_address("glPolygonOffset"));
        gl::BindBufferBase = reinterpret_cast<decltype(&glBindBufferBase)>(gl::get_proc_address("glBindBufferBase"));
        gl::BufferSubData = reinterpret_cast<decltype(&glBufferSubData)>(gl::get_proc_address("glBufferSubData"));
        gl::GetUniformBlockIndex = reinterpret_cast<decltype(&glGetUniformBlockIndex)>(gl::get_proc_address("glGetUniformBlockIndex"));
        gl::UniformBlockBinding = reinterpret_cast<decltype(&glUniformBlockBinding)>(gl::get_proc_address("glUniformBlockBinding"));
        gl::GetBooleanv = reinterpret_cast<decltype(&glGetBooleanv)>(gl::get_proc_address("glGetBooleanv"));
        gl::Finish = reinterpret_cast<decltype(&glFinish)>(gl::get_proc_address("glFinish"));
        //TODO: remove this and make some sort of extention system
#if defined(CORE_GL)
        gl::PolygonMode = reinterpret_cast<decltype(&glPolygonMode)>(gl::get_proc_address("glPolygonMode"));
        gl::PointSize = reinterpret_cast<decltype(&glPointSize)>(gl::get_proc_address("glPointSize"));
#endif
    }

    auto get_integer(GLenum name) -> GLint
    {
        GLint maxTexSize = -1;

        gl::GetIntegerv(name, &maxTexSize);

        if (maxTexSize != -1)
            return maxTexSize;
        else
            throw Exception("GetIntegerv Failed");
    }

    auto get_boolean(GLenum name) -> GLboolean
    {
        GLboolean maxTexSize = false;
        gl::GetBooleanv(name, &maxTexSize);
        return maxTexSize;
    }

}