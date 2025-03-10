#pragma once
#include <core/Global_H.hpp>
#include <windows.h>
#include <iostream>
#include <glcorearb.h> // need repl  with glext.h
#include <type_traits>
#include <string>
#include <array>
#include <source_location>
#include <tuple>
#include <utility>
#include <ranges>

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

[[maybe_unused]] inline static auto _wglMakeCurrent = decltype(&wglMakeCurrent)(nullptr);
[[maybe_unused]] inline static auto _wglCreateContext =  decltype(&wglCreateContext)(nullptr);
[[maybe_unused]] inline static auto _wglGetProcAddress = decltype(&wglGetProcAddress)(nullptr);
[[maybe_unused]] inline static auto _wglDeleteContext = decltype(&wglDeleteContext)(nullptr);
[[maybe_unused]] inline static auto _wglCopyContext = decltype(&wglCopyContext)(nullptr);
[[maybe_unused]] inline static auto _wglCreateLayerContext = decltype(&wglCreateLayerContext)(nullptr);
[[maybe_unused]] inline static auto _wglGetCurrentContext = decltype(&wglGetCurrentContext)(nullptr);
[[maybe_unused]] inline static auto _wglGetCurrentDC = decltype(&wglGetCurrentDC)(nullptr);
[[maybe_unused]] inline static auto _wglShareLists = decltype(&wglShareLists)(nullptr);
[[maybe_unused]] inline static auto _wglUseFontBitmapsA = decltype(&wglUseFontBitmapsA)(nullptr);
[[maybe_unused]] inline static auto _wglUseFontBitmapsW = decltype(&wglUseFontBitmapsW)(nullptr);

[[maybe_unused]] inline static auto wglCreateContextAttribsARB = (HGLRC(WINAPI*)(HDC, HGLRC, const int*))(nullptr);
[[maybe_unused]] inline static auto wglGetExtensionsStringARB = (const char *(WINAPI*)(HDC))(nullptr);
[[maybe_unused]] inline static auto wglSwapIntervalEXT = (BOOL(APIENTRY*)(int))(nullptr);


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

[[maybe_unused]] inline PFNGLGETERRORPROC glGetError = nullptr;

template <typename T>
class Function;

template <typename R, typename... Args>
class Function<R(APIENTRY*)(Args...)> {
public:
    using FuncType = R(APIENTRY*)(Args...);

    Function()
    : m_Func(nullptr)
    , m_Name("Function")
    , m_ReturnType(typeid(R).name())
    , m_ArgsTypes{typeid(Args).name()...}
    , m_ArgsValues{}
    , m_CallCount(0)
    { m_Count++; }

    auto operator()(Args... args, std::source_location loc = std::source_location::current()) -> R
    {
        m_ArgsValues = std::make_tuple(args...);
        m_CallCount++;
        GLenum err = GL_NO_ERROR;
        
        if constexpr (std::is_void_v<R>) {
            m_Func(args...);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cout << std::format("{} -> {} ({}) -> {}:{}", formated_function_sig(), GL_ERR_to_string(err), err, loc.file_name(), loc.line()) << std::endl;
            }
        } else {
            R result = m_Func(args...);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cout << std::format("{} -> {} ({}) -> {}:{}", formated_function_sig(), GL_ERR_to_string(err), err, loc.file_name(), loc.line()) << std::endl;
            }
            return result;
        }
    }
    auto formated_function_sig() const -> std::string
    {
        std::string result = std::format("{} {}(", m_ReturnType, m_Name);
        bool first = true;
        int index = 1;
        for(const auto& [type, value] : std::views::zip(m_ArgsTypes, ArgsValues()))
        {
            if (!first) result += ", ";
            result += type + " arg_" + std::to_string(index++) + " = " + value;
            first = false;
        }
        result += ")";
        return result;
    }

    auto ArgsValues() const -> std::array<std::string, sizeof...(Args)>
    {
        std::array<std::string, sizeof...(Args)> result;
        int i = 0;
        std::apply([this, &result, &i](auto&&... args) {
            ((result[i++] = to_string(args)), ...);
        }, m_ArgsValues);
        return result;
    }

    auto to_string(const auto& value) const -> std::string {
        using T = std::remove_reference_t<decltype(value)>;
        using CleanType = std::remove_cv_t<std::remove_pointer_t<T>>;
    
        if constexpr (std::is_pointer_v<T>) {
            if (!value) return "nullptr";
    
            if constexpr (std::is_same_v<CleanType, void>) {
                return "??";
            } else if constexpr (std::is_arithmetic_v<CleanType>) {
                return std::to_string(*value);
            } else {
                return std::string(*value);
            }
        } else if constexpr (std::is_arithmetic_v<T>) {
            return std::to_string(value);
        } else {
            return value;
        }
    }

    auto ReturnType() const -> std::string
    {
        return m_ReturnType;
    }

    auto ArgsTypes() const -> std::array<std::string, sizeof...(Args)>
    {
        return m_ArgsTypes;
    }

    auto CallsCount() const -> size_t
    {
        return m_CallCount;
    }

    constexpr auto ArgsCount() const -> size_t
    {
        return sizeof...(Args);
    }

    static auto functionCount() -> size_t
    {
        return m_Count;
    }
public:
    FuncType m_Func;
    std::string m_Name;
private:
    std::string m_ReturnType;
    std::array<std::string, sizeof...(Args)> m_ArgsTypes;
    std::tuple<Args...> m_ArgsValues;
    size_t m_CallCount;
    inline static size_t m_Count = 0;
};

#ifdef DEBUG
#   define GLFUN(type, name)\
    inline Function<type> name;
#else
#   define GLFUN(type, name)\
    inline type name;
#endif

GLFUNCS(GLFUN)
