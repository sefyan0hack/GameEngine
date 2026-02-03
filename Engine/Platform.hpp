/**
*   @file Platform.hpp
*   @author Sofyane Bentaleb
*   @brief this file has alot of macros and static functions and varables useful for platform determen and more
*/

#pragma once
#include <version>
#include <cstdint>
#include <variant>
#include <string_view>
#include <type_traits>
#include <concepts>

/**
 * @brief Custom constexpr hash function for string_view (FNV-1a algorithm)
 * @param str The string to hash
 * @return std::size_t hash value
 */
inline static constexpr std::size_t constexpr_hash(std::string_view str) noexcept {
    constexpr std::size_t offset_basis = 14695981039346656037ULL;
    constexpr std::size_t prime = 1099511628211ULL;
    
    std::size_t hash = offset_basis;
    for (char c : str) {
        hash ^= static_cast<std::size_t>(static_cast<unsigned char>(c));
        hash *= prime;
    }
    return hash;
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
 * @brief get Type hash
 * @tparam T is the type
 * @return size_t integer aka hash
 */
template <typename T>
inline static constexpr auto type_hash() noexcept -> std::size_t
{
    return constexpr_hash(::type_name<T>());
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

/**
 * @brief allot of help full info about type
 */
struct TypeInfo {
    template<class T>
    static consteval TypeInfo make() {
        return TypeInfo { 
            ::type_name<T>(),
            ::type_parent<T>(),
            ::type_kind<T>(),
            ::type_hash<T>(),
            sizeof(T),
            alignof(T),
            std::is_empty_v<T>
        };
    }

    std::string_view name;
    std::string_view parent;
    std::string_view kind;
    std::size_t      hash;
    std::size_t      size;
    std::size_t      alignment;
    bool             empty;
};

/**
 * @brief Concept that tests whether T is a Pointer
 *
 * @details
 * Example usage:
 * @code
 *
 * static_assert(Pointer<int*>);     // OK:
 * static_assert(Pointer<float*>); // OK:
 * @endcode
 */
template <typename T>
concept Pointer = std::is_pointer_v<T>;

/**
 * @brief Concept that tests whether T is a Variant
 *
 * @details
 * Example usage:
 * @code
 *
 * static_assert(Variant<std::variant<int,char>>); // OK:
 * @endcode
 */
template <typename T>
concept Variant = requires {
    typename std::variant_size<std::remove_cvref_t<T>>::type;
};

/**
 * @brief Concept that checks if T is equal to any of the listed types U...
 *
 * @tparam T The type to test
 * @tparam U Parameter pack of types to compare with T
 *
 * @returns true if T is the same as any type in U...
 *
 * @details
 * Useful in constrained templates or SFINAE situations where you want to
 * accept a set of specific types.
 */
template <typename T, typename... Allowed>
concept either = (std::convertible_to<std::remove_cvref_t<T>, Allowed> || ...);

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
