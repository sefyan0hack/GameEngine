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

inline static auto demangle(const char* name) noexcept -> std::string
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
inline static constexpr auto type_name() noexcept -> std::string_view
{
    #if defined(CLANG_CPL) || defined(GNU_CPL)
        constexpr std::string_view name = __PRETTY_FUNCTION__;
        constexpr auto start = name.find("T = ") + 4;
    #   if defined(CLANG_CPL)
        constexpr auto end = name.find(']', start);
    #   elif defined(GNU_CPL)
        constexpr auto end = name.find(';', start);
    #   endif

        return name.substr(start, end - start);

    #elif defined(MSVC_CPL)
        constexpr std::string_view name = __FUNCSIG__;
        constexpr auto start = name.find("type_name<") + 10;
        constexpr auto end = name.find(">(void)", start);
        std::string_view result = name.substr(start, end - start);
        auto newStart = result.find(' ') + 1;

        return result.substr(newStart, end - newStart);
    #else
    #   error "Compiler not Supported"
    #endif
}

inline static constexpr auto DJB2_hash(std::string_view str) -> std::size_t
{
    std::size_t hash = 5381;
    for (char c : str) {
        hash = (hash * 33) ^ static_cast<std::size_t>(c);
    }
    return hash;
}

template <typename T>
inline static constexpr auto type_hash() noexcept -> std::size_t
{
#ifdef __cpp_rtti
    return typeid(T).hash_code();
#else
    return DJB2_hash(type_name<T>());
#endif
}

inline static constexpr auto type_hash(auto type) noexcept -> std::size_t
{
#ifdef __cpp_rtti
    return typeid(type).hash_code();
#else
    return DJB2_hash(type_name<decltype(type)>());
#endif
}


template<typename T>
inline static constexpr auto type_kind()  noexcept -> std::string_view
{
    if constexpr (std::is_fundamental_v<T>)  return "primitive";
    else if constexpr (std::is_enum_v<T>)    return "enum";
    else if constexpr (std::is_union_v<T>)   return "union";
    else if constexpr (std::is_class_v<T>)   return "class/struct";
    else                                     return "unknown";
}

using TypeInfo = std::pair<std::size_t, std::string_view>;

template<class T>
struct Type {
    constexpr static std::string_view name = ::type_name<T>();
    constexpr static std::string_view kind = ::type_kind<T>();
    constexpr static std::size_t hash = ::type_hash<T>();
    constexpr static std::size_t size = sizeof(T);
    constexpr static std::size_t alignment = alignof(T);
    constexpr static bool empty = std::is_empty_v<T>;
};

template<typename T>
struct std::formatter<Type<T>, char> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Type<T>& type, std::format_context& ctx) const {
        return std::format_to(ctx.out(), 
            "{{ Type: {}, Kind: {}, Hash: {}, Size: {}, Align: {}, Empty: {} }}",
            type.name, type.kind, type.hash, type.size, type.alignment, type.empty
        );
    }
};


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
