#pragma once
#include <core/Global_H.hpp>
#include <windows.h>
#include <iostream>
#include <glcorearb.h> // need repl  with glext.h
#include <type_traits>
#include <string>
#include <sstream>
#include <unordered_map>

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

inline PFNGLGETERRORPROC glGetError = nullptr;

// Compile-time string hashing (64-bit FNV-1a)
template <size_t N>
constexpr uint64_t const_hash(const char (&str)[N]) {
    constexpr uint64_t prime = 0x100000001B3;
    uint64_t hash = 0xCBF29CE484222325;
    
    for (size_t i = 0; i < N-1; ++i) {
        hash = (hash ^ static_cast<uint64_t>(str[i])) * prime;
    }
    return hash;
}

static std::unordered_map<uint64_t, std::string>& get_hash_registry() {
    static auto* registry = new std::unordered_map<uint64_t, std::string>;
    return *registry;
}
inline std::string get_name_from_hash(uint64_t hash) {
    const auto& registry = get_hash_registry();
    auto it = registry.find(hash);
    return it != registry.end() ? it->second : "UNKNOWN_FUNCTION";
}

template <typename T, uint64_t NameHash>
struct gl_function_wrapper;

template <typename R, typename... Args, uint64_t NameHash>
struct gl_function_wrapper<R(APIENTRY*)(Args...), NameHash> {
    using FuncType = R(APIENTRY*)(Args...);
    inline static FuncType Func = nullptr;

    static R APIENTRY get_func(Args... args) {
        std::stringstream sig;
        sig << std::format("{} {}(", typeid(R).name(), get_name_from_hash(NameHash));
        (([&]() {
            sig << typeid(Args).name() << " = ";
            using CleanType = std::remove_cv_t<std::remove_reference_t<Args>>;
            if constexpr (std::is_pointer_v<CleanType>) {
                using PointeeType = std::remove_pointer_t<CleanType>;

                if constexpr (std::is_void_v<PointeeType>) {
                    sig << static_cast<const void*>(args);
                } else {
                    if(args != nullptr) sig << *args;
                    else sig << "nullptr";
                }
            } else {
                sig << args;
            }
            sig << ", ";
        }()), ...);

        auto strsig = sig.str();
        if constexpr (sizeof...(Args)){
            strsig.pop_back();
            strsig.pop_back();
        }
        strsig += ")";

        while (glGetError() != GL_NO_ERROR) {}

        GLenum err = GL_NO_ERROR;
        
        if constexpr (std::is_void_v<R>) {
            Func(args...);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cout << std::format("[OpenGL ERROR] {} : {} ({})", strsig, GL_ERR_to_string(err), err) << std::endl;
            }
        } else {
            R result = Func(args...);
            err = glGetError();
            if (err != GL_NO_ERROR) {
                std::cout << std::format("[OpenGL ERROR] {} : {} ({})", strsig, GL_ERR_to_string(err), err) << std::endl;
            }
            return result;
        }
    }
};

#define GLFUN(type, name)\
    inline type name = nullptr;

GLFUNCS(GLFUN)
