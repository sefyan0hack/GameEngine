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
 */
inline auto to_string(auto* ptr) -> std::string
{
    using Pointee = std::remove_pointer_t<std::decay_t<decltype(ptr)>>;

    if (ptr == nullptr) return "null";
    else if constexpr (std::is_pointer_v<Pointee> && !std::is_same_v<Pointee, char>) return to_string(*ptr);
    else if constexpr (std::formattable<Pointee, char>) return std::format("{}", *ptr);
    else return {};
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
