#pragma once
#include <version>
#include <type_traits>
#include <variant>
#include <utility>
#include <string>
#include <cstring>
#include <string_view>
#include <sstream>
#include <chrono>
#include <array>
#include <vector>
#include <fstream>
#include <thread>
#include <functional>
#include <ranges>

#include "Exception.hpp"

namespace utils {

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

/**
 * @brief Convert a std::variant type list to a compile-time array of Type.
 *
 * @tparam T A std::variant<...> type.
 * @return std::array<Type, N> where N == std::variant_size_v<T>
 *
 * @details
 * This helper inspects the alternative types inside a std::variant and
 * constructs an array of Type structures.
 *
 * Example:
 * @code
 * using V = std::variant<int, std::string>;
 * constexpr auto arr = utils::variant_to_array<V>();
 * @endcode
 */
template<class T>
constexpr auto variant_to_array() -> std::array<TypeInfo, std::variant_size_v<T>>
{
    return []<std::size_t... I>(std::index_sequence<I...>) {
        return std::array<TypeInfo, sizeof...(I)> {{
            TypeInfo::make<std::variant_alternative_t<I, T>>()...
        }};
    }(std::make_index_sequence<std::variant_size_v<T>>{});
}
/**
 * @brief Extracts the directory portion of a file path at compile-time.
 *
 * @param file_path File path string. Defaults to the call-site __FILE__.
 * @return A std::string_view pointing to the directory portion (excluding trailing slash),
 *         or an empty std::string_view if no directory separator is found.
 *
 * @details
 * This is a consteval helper intended for compile-time extraction of the containing
 * directory from a path literal. Works with both '/' and '\\' separators.
 *
 * Example:
 * @code
 * constexpr auto dir = utils::get_file_directory("/path/to/file.cpp");
 * // dir == "/path/to"
 * @endcode
 */
consteval auto get_file_directory(const std::string_view file_path = __FILE__) -> std::string_view
{
    const std::size_t last_slash = file_path.find_last_of("/\\");
    return (last_slash == std::string_view::npos) ? "" : file_path.substr(0, last_slash);
}

/**
 * @brief Simple compile-time fixed string wrapper usable as a NTTP.
 *
 * @tparam N Size of the stored char array (including terminating null).
 *
 * @details
 * Constructible from string literal arrays. Intended to be used with
 * compile-time string processing templates and concepts.
 */
template <std::size_t N>
struct FixedString {
    constexpr FixedString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    char value[N];
};

/**
 * @brief Compile-time transform that converts a FixedString to uppercase.
 *
 * @tparam Str A FixedString non-type template parameter.
 *
 * @details
 * This template computes an std::array<char, N> where each ASCII lowercase
 * letter is converted to uppercase. The last character remains the null
 * terminator. Use ToUpper<FixedString{"..."}>::value to access the array.
 */
template <FixedString Str>
struct ToUpper {
    static constexpr auto compute() {
        constexpr std::size_t N = sizeof(Str.value);
        std::array<char, N> result{};
        for (std::size_t i = 0; i < N - 1; ++i) {
            char c = Str.value[i];
            if (c >= 'a' && c <= 'z') {
                result[i] = c - ('a' - 'A');
            } else {
                result[i] = c;
            }
        }
        result[N - 1] = '\0';
        return result;
    }
    static constexpr std::array<char, sizeof(Str.value)> value = compute();
};

template <FixedString Str>
constexpr std::array<char, sizeof(Str.value)> ToUpper<Str>::value;

/**
 * @brief Helper macro to produce a compile-time uppercase std::string_view.
 *
 * @param str Literal string to convert to uppercase at compile time.
 *
 * @details
 * Expands to a std::string_view referencing the compile-time uppercased data.
 * Example: to_upper("hello") yields "HELLO".
 */
template <FixedString Str>
constexpr std::string_view to_upper() {
    return std::string_view{
        ToUpper<Str>::value.data(),
        ToUpper<Str>::value.size() - 1 // exclude null terminator
    };
}

/**
 * @brief Read an entire file into a std::string.
 *
 * @param path File system path to the file.
 * @return std::string The file contents.
 *
 * @throws Exception If the file cannot be opened. The exception includes
 *                    strerror(errno) for diagnostic purposes.
 */
inline auto file_to_str(const char* path) -> std::string
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    
    if (!file) { 
        throw Exception("Couldn't open file [{}] : {}", path, std::strerror(errno));
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
 * @brief Read an entire file into a std::vector<std::byte>.
 *
 * @param path File system path to the file.
 * @return std::vector<std::byte> The file bytes.
 *
 * @details
 * Internally calls file_to_str and casts the underlying data to std::byte.
 */
inline auto file_to_vec(const char* path) -> std::vector<std::byte>
{
    auto filestr = file_to_str(path);
    const auto* data_start = reinterpret_cast<const std::byte*>(filestr.data());
    return std::vector<std::byte>(data_start, data_start + filestr.size());
}

/**
 * @brief Format raw memory (pointer + count) as a hex string.
 *
 * @tparam T Element type.
 * @param data Pointer to the first element.
 * @param n Number of elements (not bytes) to format.
 * @return std::string Human-readable hex dump of the memory region.
 *
 * @details
 * The function treats the memory as a sequence of unsigned bytes and formats
 * each byte in hex. The total number of bytes processed is n * sizeof(T).
 */
template <class T>
inline auto to_hex(T* data, std::size_t n) -> std::string
{
    static_assert(std::is_trivially_copyable_v<T>, "to_hex requires trivially copyable types for safe byte access");

    std::string res;
    for (auto const& b : std::as_bytes(std::span(data, n)) ) {
        if (!res.empty()) res += ' ';
        res += ::format("0x{:02X}", static_cast<uint8_t>(b));
    }
    return res;
}

/**
 * @brief Format a pointer to a single object as a hex string.
 *
 * @tparam T Pointee type.
 * @param data Pointer to the object.
 * @return std::string Hex dump of the object bytes (size determined by sizeof(T)).
 */
template <class T>
inline auto to_hex(T* data) -> std::string
{
    return to_hex(data, 1);
}

/**
 * @brief Format arbitrary data as a hex string (single-byte granularity).
 *
 * @tparam T Data element type.
 * @param data Reference to the data element.
 * @return std::string Human-readable hex dump of the object bytes.
 *
 * @details
 * Equivalent to calling to_hex(&data, 1). Uses std::stringstream with
 * std::hex, std::setw(2) and std::setfill('0') to format bytes. Groups
 * bytes with two spaces after every 8 bytes for readability.
 */
template <class T>
inline auto to_hex(const T& data) -> std::string
{
    return to_hex(&data, 1);
}

/**
 * @brief Format a C-style array as a hex string.
 *
 * @tparam T Element type.
 * @tparam N Number of elements in the array.
 * @param data Reference to the array.
 * @return std::string Hex dump of the whole array (N * sizeof(T) bytes).
 */
template <class T, std::size_t N>
inline auto to_hex(const T (&data)[N]) -> std::string
{
    return to_hex(data, N);
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
auto pointer_to_string(auto* ptr) -> std::string
{
    using Pointee = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;
    constexpr const char* r = "*({}){}";

    constexpr auto type = ::type_name<Pointee>();

    if (ptr == nullptr) return "null";
    else if constexpr (std::is_pointer_v<Pointee>) return pointer_to_string(*ptr);
    else if constexpr (std::is_same_v<Pointee, void>) return ::format(r, type, "??");
    else if constexpr (std::formattable<Pointee, char>) return ::format(r, type, *ptr);
    else if constexpr (requires(std::ostream& os) { os << *ptr; }) {
        std::stringstream ss;
        ss << *ptr;
        return ::format(r, type, ss.str());
    }
    else if constexpr (std::is_function_v<Pointee>) return ::format(r, type, "");
    else return ::format(r, type, to_hex(ptr));
}

template<class TVarinat, class... TMatchers>
    requires(sizeof...(TMatchers) >= 1 )
inline auto match(TVarinat&& v, TMatchers&&... m) -> decltype(auto)
{
    return std::visit(
        overloaded {std::forward<TMatchers>(m)...},
        std::forward<TVarinat>(v)
    );
}


/**
 * @brief Repeatedly call a function at a fixed interval.
 *
 * @tparam Function Callable type.
 * @tparam Args Types of arguments forwarded to the callable.
 * @param interval Interval in milliseconds between invocations.
 * @param func Callable to invoke repeatedly.
 * @param args Arguments to forward to func.
 *
 * @details
 * Launches a detached thread that calls the provided callable in an
 * infinite loop, sleeping for `interval` milliseconds between calls.
 * Use with caution — the loop is infinite and will run until the program exits.
 *
 * Example:
 * @code
 * utils::async_repeat_every(1000, [](){ puts("every second"); });
 * @endcode
 */

inline auto async_repeat_every(int64_t ms, std::invocable auto F, auto&&... args) -> void
{
    std::thread([=](){
        while(true){
            F(args...);
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }
    }).detach();
}


/**
 * @brief Schedule a callable to run once after a delay (milliseconds).
 *
 * @tparam Function Callable type.
 * @tparam Args Types of arguments forwarded to the callable.
 * @param delay Delay in milliseconds before invocation.
 * @param func Callable to invoke.
 * @param args Arguments to forward to func.
 *
 * @details
 * This will launch a detached std::thread which sleeps for `delay`
 * milliseconds and then invokes the callable with the provided arguments.
 * The thread is detached; errors thrown inside the callable are not propagated.
 *
 * Example:
 * @code
 * utils::async_run_after(500, [](){ puts("hello after 500ms"); });
 * @endcode
 */

inline auto async_run_after(int64_t ms, std::invocable auto F, auto&&... args) -> void
{
    std::thread([ms, F = std::forward<decltype(F)>(F), ...args = std::forward<decltype(args)>(args)](){
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        F(args...);
    }).detach();
}

inline auto async_while(bool& cond, std::invocable auto F, auto&&... args) -> void
{
    std::thread([cond, F = std::forward<decltype(F)>(F), ...args = std::forward<decltype(args)>(args)](){
        while(cond){
            F(args...);
        }
    }).detach();
}

}// namespace utils
