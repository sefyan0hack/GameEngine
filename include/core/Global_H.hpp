#pragma once
#if !defined _MSC_VER
#pragma GCC system_header
#endif

// disable warning
#ifdef _MSC_VER
    #define NO_WARNING_BEGIN \
        __pragma(warning(push, 0))
    #define NO_WARNING_END \
        __pragma(warning(pop))
#elif defined(__GNUC__)
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

#ifdef TESTING
#define FOR_TEST public: template<unsigned short n> struct Test;
#else
#define FOR_TEST
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   define WINDOWS_PLT
#elif defined(__linux__)
#   define LINUX_PLT
#endif

#include <type_traits>
template<auto var>
concept is_static = std::is_object_v<std::remove_pointer_t<decltype(var)>> && !std::is_member_object_pointer_v<decltype(var)>;

#ifndef MEMBER_VAR
#define MEMBER_VAR(Var) \
    const auto& Var = [&]() -> const auto& { \
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


#if defined(__clang__) && !defined(WINDOWS_PLT) || defined(__GNUC__) || defined(__EDG__)
#include <cxxabi.h>
#endif

#include <cstdlib>
#include <memory>
#include <string>

inline static auto demangle(const char* name) -> std::string
{
    [[maybe_unused]] int status = -1;
    #if defined(__clang__) && !defined(WINDOWS_PLT) || defined(__GNUC__) || defined(__EDG__)
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
    #if defined(__clang__) || defined(__GNUC__) || defined(__EDG__)
        std::string name = __PRETTY_FUNCTION__;
        auto start = name.find("T = ") + 4;
    #   if defined(__clang__) || defined(__EDG__)
        auto end = name.find(']', start);
    #   elif defined(__GNUC__)
        auto end = name.find(';', start);
    #   endif

        return std::string(name.substr(start, end - start));

    #elif defined(_MSC_VER)
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