/**
*   @file Platform.hpp
*   @author Sofyane Bentaleb
*   @brief this file has alot of macros and static functions and varables useful for platform determen and more
*/

#pragma once
// #include <version>
// #include <type_traits>
// #include <variant>
// #include <format>
// #include <string_view>
// #include <utility>
// #include <cstdlib>
// #include <memory>
// #include <string>

#ifdef __has_include
  #if __has_include(<version>)
    #include <version>
  #endif

  #if __has_include(<type_traits>)
    #include <type_traits>
  #endif

  #if __has_include(<variant>)
    #include <variant>
  #endif

  #if __has_include(<format>)
    #include <format>
  #endif

  #if __has_include(<string_view>)
    #include <string_view>
  #endif

  #if __has_include(<utility>)
    #include <utility>
  #endif

  #if __has_include(<cstdlib>)
    #include <cstdlib>
  #endif

  #if __has_include(<memory>)
    #include <memory>
  #endif

  #if __has_include(<string>)
    #include <string>
  #endif

  #if __has_include(<cxxabi.h>)
    #include <cxxabi.h>
  #endif
  
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


#define DECLARE_FUNCTION(name, r, args) auto name args -> r


#define PRAGMA_TO_STR(x)        #x
#define PRAGMA_STR(x)           PRAGMA_TO_STR(GCC diagnostic ignored x)


#if defined(MSVC_CPL)
    /**
     * @brief Pushes the current warning state, disables the specified MSVC warning,
     * and makes subsequent code portable by nullifying GCC/Clang macros.
     * @param w The numeric warning code (e.g., 4996).
     */
    #define BEING_IGNORE_WARN_MSVC(w)     __pragma(warning(push)) __pragma(warning(disable: w))
    #define END_IGNORE_WARN_MSVC()     __pragma(warning(pop))

    #define BEING_IGNORE_WARN_GCC(w)
    #define END_IGNORE_WARN_GCC()
    #define BEGIN_IGNORE_WARN_CALNG(w)
    #define END_IGNORE_WARN_CALNG()
#endif


#if defined(GCC_CPL) && !defined(CLANG_CPL)
    /**
     * @brief Pushes the current diagnostic state, disables the specified GCC warning,
     * and makes subsequent code portable by nullifying MSVC/Clang macros.
     * @param w The string warning flag (e.g., "-Wdeprecated-declarations").
     */
    #define BEING_IGNORE_WARN_GCC(w)      _Pragma("GCC diagnostic push") _Pragma(PRAGMA_STR(w))
    #define END_IGNORE_WARN_GCC()      _Pragma("GCC diagnostic pop")

    #define BEING_IGNORE_WARN_MSVC(w)
    #define END_IGNORE_WARN_MSVC()
    #define BEGIN_IGNORE_WARN_CALNG(w)
    #define END_IGNORE_WARN_CALNG()
#endif


#if defined(CLANG_CPL)
    /**
     * @brief Pushes the current diagnostic state, disables the specified Clang warning,
     * and makes subsequent code portable by nullifying MSVC/GCC macros.
     * @param w The string warning flag (e.g., "-Wdeprecated-declarations").
     */
    #define BEGIN_IGNORE_WARN_CALNG(w)    _Pragma("GCC diagnostic push") _Pragma(PRAGMA_STR(w))
    #define END_IGNORE_WARN_CALNG()    _Pragma("GCC diagnostic pop")

    #define BEING_IGNORE_WARN_MSVC(w)
    #define END_IGNORE_WARN_MSVC()
    #define BEING_IGNORE_WARN_GCC(w)
    #define END_IGNORE_WARN_GCC()
#endif


#if !defined(MSVC_CPL) && !defined(GCC_CPL) && !defined(CLANG_CPL)
    #define BEING_IGNORE_WARN_MSVC(w)
    #define END_IGNORE_WARN_MSVC()
    #define BEING_IGNORE_WARN_GCC(w)
    #define END_IGNORE_WARN_GCC()
    #define BEGIN_IGNORE_WARN_CALNG(w)
    #define END_IGNORE_WARN_CALNG()
#endif


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
 * @brief Concept that tests whether T is a Function Pointer
 *
 * @details
 * Example usage:
 * @code
 *
 * static_assert(Pointer<void(f*)(int)>); // OK:
 * @endcode
 */
template<typename T>
concept FunctionPointer =
    std::is_pointer_v<std::remove_cv_t<T>> &&
    std::is_function_v<std::remove_pointer_t<std::remove_cv_t<T>>>;

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


template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

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

/**
 * @brief Concept that tests whether T is Formattable
 *
 * @details
 * Example usage:
 * @code
 *
 * static_assert(formattable<char>); // OK:
 * @endcode
 */
template<class T> concept formattable = std::formattable<T, char>;