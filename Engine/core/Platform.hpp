/**
*   @file Platform.hpp
*   @author Sofyane Bentaleb
*   @brief this file has alot of macros and static functions and varables useful for platform determen and more
*/

#pragma once
#include <version>
#include <type_traits>
#include <string_view>
#include <utility>
#include <cstdlib>
#include <memory>
#include <string>

#if defined(CLANG_CPL) && !defined(WINDOWS_PLT) || defined(GNU_CPL)
#include <cxxabi.h>
#endif


/**
 * @brief  Concept that tests whether a non-type template parameter names a free/static object
 *         (or pointer to one) and is **not** a pointer-to-member-object.
 *
 * @tparam var  A non-type template parameter. The concept is satisfied when
 *              `std::is_object_v<std::remove_pointer_t<decltype(var)>>` is true
 *              and `decltype(var)` is not a member-object-pointer.
 *
 * @details
 * Example usage:
 * @code
 * static int global_int = 0;
 * struct S { int m; };
 *
 * static_assert(is_static<global_int>);     // OK: global_int is an object
 * static_assert(is_static<&global_int>);    // OK: pointer to an object
 * static_assert(!is_static<&S::m>);         // false: pointer-to-member is excluded
 * @endcode
 */
template<auto var>
concept is_static = std::is_object_v<std::remove_pointer_t<decltype(var)>> && !std::is_member_object_pointer_v<decltype(var)>;

/** 
 * @brief demangle c++ symbole   only for clang/linux and gcc/(windows, linux)
 * @param name mangled name
 * @return demangle name if name is vaid mangled c++ else get back name it self
 */
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
/**
 * @brief get Type name in string form
 * @tparam T is the type
 * @return string view aka "T"
 */
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
/**
 * @brief simple DJB2 hash algorithm
 * @param str string to hash
 * @return size_t integer aka hash
 */
inline static constexpr auto DJB2_hash(std::string_view str) noexcept -> std::size_t
{
    std::size_t hash = 5381;
    for (char c : str) {
        hash = (hash * 33) ^ static_cast<std::size_t>(c);
    }
    return hash;
}

/**
 * @brief FNV-1a 64-bit hash function
 * @param str string to hash
 * @return size_t integer aka hash
 */
inline static constexpr auto fnv1a_hash(std::string_view str) noexcept -> std::size_t
{
    std::size_t result = sizeof(std::size_t) == 4 ? 0x811C9DC5u : 0xCBF29CE484222325u;
    constexpr std::size_t prime = sizeof(std::size_t) == 4 ? 0x01000193u : 0x00000100000001B3u;

    for (char c : str) {
        const auto byte = static_cast<unsigned char>(c);
        result = (result ^ static_cast<std::size_t>(byte)) * prime;
    }
    return result;
}

/**
 * @brief get Type hash
 * @tparam T is the type
 * @return size_t integer aka hash
 */
template <typename T>
inline static constexpr auto type_hash() noexcept -> std::size_t
{
    return fnv1a_hash(type_name<T>());
}

/**
 * @brief get Type hash take that argument aka varable and the type is decltype(type)
 * @tparam type variable of type decltype(type)
 * @return size_t integer aka hash
 */    
inline static constexpr auto type_hash(auto type) noexcept -> std::size_t
{
    return type_hash<decltype(type)>();
}

/**
 * @brief get the king of the T
 * @tparam T is the type
 * @return a string one of of `primitive/class/enum/union`
 */
template<typename T>
inline static constexpr auto type_kind()  noexcept -> std::string_view
{
    if constexpr (std::is_fundamental_v<T>)  return "primitive";
    else if constexpr (std::is_enum_v<T>)    return "enum";
    else if constexpr (std::is_union_v<T>)   return "union";
    else if constexpr (std::is_class_v<T>)   return "class/struct";
    else                                     return "unknown";
}

/**
 * @brief get the parent aka base class or namespace before `*::T`
 * @tparam T is the type
 * @return the parent is eather a type or some type of namespace
 */
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

/**
 * @brief allot of help full info about type
 * @tparam T the Type
 */
template<class T>
struct Type {
    constexpr static std::string_view name = ::type_name<T>();      /*!< type name of T */
    constexpr static std::string_view parent = ::type_parent<T>();  /*!< type parent name of T */
    constexpr static std::string_view kind = ::type_kind<T>();      /*!< type kind name of T */
    constexpr static std::size_t      hash = ::type_hash<T>();      /*!< type hash name of T */
    constexpr static std::size_t      size = sizeof(T);             /*!< type size name of T */
    constexpr static std::size_t      alignment = alignof(T);       /*!< type allingment name of T */
    constexpr static bool             empty = std::is_empty_v<T>;   /*!< is T empty type or not */
};
