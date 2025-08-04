#pragma once
#if !defined(MSVC_CPL)
#pragma GCC system_header
#endif

/// @brief Disable warning Macro
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

/// @brief Get correct res directory for Texture and Shader
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

/// @brief Add sub class needed for testing to access non public members
#define FOR_TEST public: template<uint16_t n> struct Test;

#include <type_traits>
/// @brief Check if a given variable is static 
template<auto var>
concept is_static = std::is_object_v<std::remove_pointer_t<decltype(var)>> && !std::is_member_object_pointer_v<decltype(var)>;

#ifndef MEMBER_VAR
/// @brief declare member variable in interface to test need it as a hack
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
/// @brief declare member function in interface to test need it as a hack
#   define MEMBER_FUN(Name) auto Name(auto&&... args) { return member.Name(std::forward<decltype(args)>(args)...); }
#endif

#ifndef MEMBER_OPCAST
/// @brief declare cast operator in interface to test need it as a hack
#   define MEMBER_OPCAST(Type) operator Type() const noexcept { return member.operator Type(); }
#endif


#if defined(CLANG_CPL) && !defined(WINDOWS_PLT) || defined(GNU_CPL)
#include <cxxabi.h>
#endif

#include <cstdlib>
#include <memory>
#include <string>

/// @brief demangle c++ symbole   only for clang/linux and gcc/(win32, linux)
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

/// @brief get Type name in string form
/// @param T is the type
/// @return string view aka "T"
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

/// @brief simple DJB2 hash algorithm
/// @param str string to hash
/// @return size_t integer aka hash
inline static constexpr auto DJB2_hash(std::string_view str) noexcept -> std::size_t
{
    std::size_t hash = 5381;
    for (char c : str) {
        hash = (hash * 33) ^ static_cast<std::size_t>(c);
    }
    return hash;
}

/// @brief FNV-1a 64-bit hash function
/// @param str string to hash
/// @return size_t integer aka hash
inline static constexpr auto (std::string_view str) noexcept -> std::size_t
{
    std::size_t result = sizeof(std::size_t) == 4 ? 0x811C9DC5u : 0xCBF29CE484222325u;
    constexpr std::size_t prime = sizeof(std::size_t) == 4 ? 0x01000193u : 0x00000100000001B3u;

    for (char c : str) {
        const auto byte = static_cast<unsigned char>(c);
        result = (result ^ static_cast<std::size_t>(byte)) * prime;
    }
    return result;
}

/// @brief get Type hash
/// @param T is the type
/// @return size_t integer aka hash
template <typename T>
inline static constexpr auto type_hash() noexcept -> std::size_t
{
    return fnv1a_hash(type_name<T>());
}

/// @brief get Type hash take that argument aka varable and the type is decltype(type)
/// @param type variable of type decltype(type)
/// @return size_t integer aka hash
inline static constexpr auto type_hash(auto type) noexcept -> std::size_t
{
    return type_hash<decltype(type)>();
}

/// @brief get the king of the T
/// @param T is the type
/// @return a string one of of `primitive/class/enum/union`
template<typename T>
inline static constexpr auto type_kind()  noexcept -> std::string_view
{
    if constexpr (std::is_fundamental_v<T>)  return "primitive";
    else if constexpr (std::is_enum_v<T>)    return "enum";
    else if constexpr (std::is_union_v<T>)   return "union";
    else if constexpr (std::is_class_v<T>)   return "class/struct";
    else                                     return "unknown";
}

/// @brief get the parent aka base class or namespace before `*::T`
/// @param T is the type
/// @return the parent is eather a type or some type of namespace
template<typename T>
inline static constexpr auto type_parent() noexcept -> std::string_view
{
    constexpr auto type = ::type_name<T>();
    constexpr auto end = type.find("::");
    if constexpr (end != std::string_view::npos)
        return type.substr(0, end);
    else
        return "";
}

/// @brief Typeinfo holdes a hash and a name
using TypeInfo = std::pair<std::size_t, std::string_view>;

/// @brief allot of help full info about type
template<class T>
struct Type {
    /// @brief type name of T
    constexpr static std::string_view name = ::type_name<T>();

    /// @brief  type parent name of T
    constexpr static std::string_view parent = ::type_parent<T>();

    /// @brief  type kind name of T
    constexpr static std::string_view kind = ::type_kind<T>();

    /// @brief  type hash name of T
    constexpr static std::size_t hash = ::type_hash<T>();

    /// @brief  type size name of T
    constexpr static std::size_t size = sizeof(T);

    /// @brief  type allingment name of T
    constexpr static std::size_t alignment = alignof(T);

    /// @brief  is T empty type or not
    constexpr static bool empty = std::is_empty_v<T>;
};

/// @brief sys name space has some usefull info about the system host
namespace sys {
    /// @brief enum of Target systems
    enum class Target : uint8_t
    {
        Windows,
        Linux,
        Web,
        Unknown
    };

    /// @brief enum of Arch systems
    enum class Arch : uint8_t
    {
        x86_64,
        Arm64,
        x86,
        Arm,
        Wasm,
        Unknown
    };

    /// @brief Target system built for
    #if defined(WINDOWS_PLT)
    constexpr auto Target = sys::Target::Windows;
    #elif defined(LINUX_PLT)
    constexpr auto Target = sys::Target::Linux;
    #elif defined(WEB_PLT)
    constexpr auto Target = sys::Target::Web;
    #else
    constexpr auto Target = sys::Target::Unknown;
    #endif

    /// @brief Arch system built for
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

    /// @brief Target system in string form
    constexpr const char* TargetName = 
        Target == Target::Windows ? "Windows" :
        Target == Target::Linux   ? "Linux"   :
        Target == Target::Web     ? "Web"     : "UNKNOWN";

    /// @brief Arch in string form
    constexpr const char* ArchName =
        Arch == Arch::x86_64 ? "x86_64" :
        Arch == Arch::Arm64  ? "Arm64"  :
        Arch == Arch::x86    ? "x86"    :
        Arch == Arch::Arm    ? "Arm"    :
        Arch == Arch::Wasm   ? "Wasm"   : "UNKNOWN";

    /// @brief Time stamp of the build
    constexpr const char* TimeStamp = __TIMESTAMP__;
} //namespace sys
