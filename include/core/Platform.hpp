#pragma once
#if !defined(MSVC_CPL)
#pragma GCC system_header
#endif

// disable warning
#if defined(MSVC_CPL)
    #define NO_WARNING_BEGIN \
        __pragma(warning(push, 0))
    #define NO_WARNING_END \
        __pragma(warning(pop))
#elif defined(GNU_CPL)
    #define NO_WARNING_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"") \
        _Pragma("GCC diagnostic ignored \"-Wextra\"") \
        _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
        _Pragma("GCC diagnostic ignored \"-Wconversion\"") \
        _Pragma("GCC diagnostic ignored \"-Wshadow\"") \
        _Pragma("GCC diagnostic ignored \"-Wcast-align\"") \
        _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
        _Pragma("GCC diagnostic ignored \"-w\"")
    #define NO_WARNING_END \
        _Pragma("GCC diagnostic pop")
#else
    #define NO_WARNING_BEGIN
    #define NO_WARNING_END
#endif

#ifdef PROJECT_SOURCE_DIR
    #define TEXTURE(str)  PROJECT_SOURCE_DIR"/res/"#str
    #define SHADER(str)   PROJECT_SOURCE_DIR"/res/Shaders/"#str
#else
    #define TEXTURE(str) #str
    #define SHADER(str)  #str
#endif

#ifndef OPENGL_LIB
    #error "no opengl lib path"
#endif

#define FOR_TEST public: template<uint16_t n> struct Test;

#include <type_traits>
template<auto var>
concept is_static = std::is_object_v<std::remove_pointer_t<decltype(var)>> && !std::is_member_object_pointer_v<decltype(var)>;

#ifndef MEMBER_VAR
#define MEMBER_VAR(Var) \
    const std::remove_reference_t<decltype(member.Var)>& Var = [&]() -> const std::remove_reference_t<decltype(member.Var)>& { \
        using class_type = std::remove_cvref_t<decltype(member)>; \
        if constexpr (is_static<&class_type::Var>) { \
            static const auto& ref = member.Var; \
            return ref; \
        } else { \
            return member.Var; \
        } \
    }()
#endif

#ifndef MEMBER_FUN
#   define MEMBER_FUN(Name) auto Name(auto&&... args) { return member.Name(std::forward<decltype(args)>(args)...); }
#endif

#ifndef MEMBER_OPCAST
#   define MEMBER_OPCAST(Type) operator Type() const noexcept { return member.operator Type(); }
#endif


#if defined(CLANG_CPL) && !defined(WINDOWS_PLT) || defined(GNU_CPL)
#include <cxxabi.h>
#endif

#include <cstdlib>
#include <memory>
#include <string>

inline static auto demangle(const char* name) -> std::string
{
    [[maybe_unused]] int32_t status = -1;
    #if defined(CLANG_CPL) && !defined(WINDOWS_PLT) || defined(GNU_CPL)
    [[maybe_unused]] std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free
    };
    return (status == 0) ? res.get() : name;
    #else
    return name;
    #endif
}


template <typename T>
inline static auto type_name() -> std::string
{
#ifdef __cpp_rtti
    return ::demangle(typeid(T).name());
#else
    #if defined(CLANG_CPL) || defined(GNU_CPL)
        std::string name = __PRETTY_FUNCTION__;
        auto start = name.find("T = ") + 4;
    #   if defined(CLANG_CPL)
        auto end = name.find(']', start);
    #   elif defined(GNU_CPL)
        auto end = name.find(';', start);
    #   endif

        return std::string(name.substr(start, end - start));

    #elif defined(MSVC_CPL)
        std::string name = __FUNCSIG__;
        auto start = name.find("type_name<") + 10;
        auto end = name.find(">(void)");
        std::string result = std::string(name.substr(start, end - start));

        static const std::string prefixes[] = {"class ", "struct ", "union ", "enum "};
        for (const auto& prefix : prefixes) {
            if (result.rfind(prefix, 0) == 0) {
                result.erase(0, prefix.length());
                break;
            }
        }
        return result;
    #endif
#endif
}

namespace sys {
    enum class Target : uint8_t
    {
        Windows,
        Linux,
        Web,
        Unknown
    };

    enum class Arch : uint8_t
    {
        x86_64,
        Arm64,
        x86,
        Arm,
        Wasm,
        Unknown
    };

    #if defined(WINDOWS_PLT)
    constexpr auto Target = sys::Target::Windows;
    #elif defined(LINUX_PLT)
    constexpr auto Target = sys::Target::Linux;
    #elif defined(WEB_PLT)
    constexpr auto Target = sys::Target::Web;
    #else
    constexpr auto Target = sys::Target::Unknown;
    #endif

    #if   defined(__x86_64__)  || defined(_M_AMD64)
        constexpr auto Arch = sys::Arch::x86_64;
    #elif defined(__aarch64__) || defined(_M_ARM64)
        constexpr auto Arch = sys::Arch::Arm64;
    #elif defined(__i386__)    || defined(_M_IX86)
        constexpr auto Arch = sys::Arch::x86;
    #elif defined(__arm__)     || defined(_M_ARM)
        constexpr auto Arch = sys::Arch::Arm;
    #elif defined(__wasm32__)  || defined(__wasm__)
        constexpr auto Arch = sys::Arch::Wasm;
    #else
        constexpr auto Arch = sys::Arch::Unknown;
    #endif

    constexpr const char* TargetName = 
        Target == Target::Windows ? "Windows" :
        Target == Target::Linux   ? "Linux"   :
        Target == Target::Web     ? "Web"     : "UNKNOWN";

    constexpr const char* ArchName =
        Arch == Arch::x86_64 ? "x86_64" :
        Arch == Arch::Arm64  ? "Arm64"  :
        Arch == Arch::x86    ? "x86"    :
        Arch == Arch::Arm    ? "Arm"    :
        Arch == Arch::Wasm   ? "Wasm"   : "UNKNOWN";

    constexpr const char* TimeStamp = __TIMESTAMP__;
} //namespace sys

#if defined(__cpp_exceptions) || defined(_CPPUNWIND) || defined(__EXCEPTIONS)
#define CXX_EXCEPTIONS
#endif

#if defined(CXX_EXCEPTIONS)
#define IF_THROWS_IGNOR(...) try {__VA_ARGS__} catch(...){}
#else
#define IF_THROWS_IGNOR(...) __VA_ARGS__
#endif
