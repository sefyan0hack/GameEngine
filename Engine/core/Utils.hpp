#pragma once
#include <version>
#include <type_traits>
#include <utility>
#include <variant>
#include <string>
#include <format>
#include <fstream>

#include "Exception.hpp"

namespace utils {

/**
 * @brief Read an entire file into a std::string.
 *
 * @param path File system path to the file.
 * @return std::string The file contents.
 *
 * @throws Exception If the file cannot be opened. The exception includes
 */
inline auto file_to_str(const char* path) -> std::string
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    
    if (!file) {
        throw Exception("Couldn't open file [{}] : errno {}", path, errno);
    }
    
    std::streamsize size = file.tellg();
    if (size <= 0) {
        return "";
    }
    
    file.seekg(0, std::ios::beg);
    std::string content;
    content.resize(static_cast<size_t>(size));
    
    if (!file.read(content.data(), size)) {
        throw Exception("Failed to read file [{}]", path);
    }
    
    return content;
}

/**
 * @brief Produce a human-friendly string description of a pointer.
 *
 * @tparam P A pointer type that satisfies the Pointer concept.
 * @param ptr The pointer to describe.
 * @return std::string String describing the pointer contents, nullness, or address.
 *
 * @details
 * - If `ptr` is `nullptr` -> returns `"null"`.
 * - If the pointer type is directly constructible into `std::string` -> returns that string.
 * - If the pointed-to type is a function -> returns the function type name.
 * - If the pointee is formattable -> returns `::format("{}", *ptr)`.
 * - If the pointee is `void` -> returns an address formatted as `&lt;{:p}&gt; : void*`.
 * - Otherwise returns formatted `&lt;{:p}&gt; (size) : [ hex-dump ]`.
 *
 */
inline auto pointer_to_string(auto* ptr) -> std::string
{
    using Pointee = std::decay_t<decltype(ptr)>;
    constexpr const char* r = "*({}){}";

    constexpr auto type = typeid(Pointee).name();

    if (ptr == nullptr) return "null";
    else if constexpr (std::is_pointer_v<Pointee>) return pointer_to_string(*ptr);
    else if constexpr (std::is_same_v<Pointee, void>) return std::format(r, type, "void??");
    else if constexpr (std::formattable<Pointee, char>) return std::format(r, type, *ptr);
    else if constexpr (std::is_function_v<Pointee>) return std::format(r, type, "");
    else return std::format(r, type, "??");
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class TVarinat, class... TMatchers>
    requires(sizeof...(TMatchers) >= 1 )
inline auto match(TVarinat&& v, TMatchers&&... m) -> decltype(auto)
{
    return std::visit(
        overloaded {std::forward<TMatchers>(m)...},
        std::forward<TVarinat>(v)
    );
}

}// namespace utils
