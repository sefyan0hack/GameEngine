#pragma once
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
#include <future>
#include <functional>

#include "Log.hpp"
#include "Exception.hpp"
#include "Platform.hpp"

#if defined(WINDOWS_PLT)
#include <windows.h>
#undef near
#undef far
#elif defined(LINUX_PLT)
#include <dlfcn.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
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
 * static_assert(std::variant<int,char>); // OK:
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
template<typename T, typename ... U>
concept either = (std::same_as<T, U> || ...);

namespace utils {

/**
 * @brief Convert a std::variant type list to a compile-time array of TypeInfo.
 *
 * @tparam T A std::variant<...> type.
 * @return std::array<TypeInfo, N> where N == std::variant_size_v<T>
 *
 * @details
 * This helper inspects the alternative types inside a std::variant and
 * constructs an array of TypeInfo structures (assumes ::type_hash<T>()
 * and ::type_name<T>() are available for each alternative type).
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
            { ::type_hash<std::variant_alternative_t<I, T>>(),
              ::type_name<std::variant_alternative_t<I, T>>()
            }...
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
 * @brief Check whether an integral value is odd.
 *
 * @param x Value to test (any type supporting % and comparison).
 * @return true if x is odd, false otherwise.
 *
 * @details
 * A constexpr helper for simple odd/even checks.
 */

constexpr auto is_odd(auto x) -> bool
{
    return x % 2 != 0;
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
 * Example: TO_UPPER("hello") yields "HELLO".
 */
#define TO_UPPER(str) std::string_view(ToUpper<FixedString{str}>::value.data())


/**
 * @brief Asynchronously load a file into a vector<char>.
 *
 * @param filename Path to the file to read.
 * @return std::future<std::vector<char>> A future that will contain the file bytes.
 *
 * @throws Exception if the file cannot be opened or is larger than vector capacity.
 *
 * @details
 * The file is opened in binary mode and read into a contiguous vector<char>.
 * The operation runs on a separate thread via std::async(launch::async).
 */
inline std::future<std::vector<char>> load_file_async(const std::string& filename) {
    using namespace std;
    return async(launch::async, [filename]() -> vector<char> {
        ifstream file(filename, ios::binary | ios::ate);
        if (!file)
            throw Exception("Cant open file {}", filename);

        streamsize size = static_cast<std::streamsize>(file.tellg());
        file.seekg(0, ios::beg);
 
        if (static_cast<std::size_t>(size) > vector<char>().max_size()){
            throw Exception("file too big {}", size);
        }

        vector<char> buffer(static_cast<std::size_t>(size));
        file.read(buffer.data(), size);
        return buffer;
    });
}

/**
 * @brief Split a C-string by a delimiter into a vector of std::string.
 *
 * @param cstr Null-terminated C string to split. If nullptr, returns empty vector.
 * @param delimiter The delimiter string used to split.
 * @return std::vector<std::string> Tokens resulting from the split.
 *
 * @details
 * If the input string is empty the function returns an empty vector.
 * The delimiter may be more than one character.
 */
inline std::vector<std::string> split(const char* cstr, const std::string& delimiter) {
    using namespace std;
    if (cstr == nullptr) return {};
    string s(cstr);
    
    if(s.empty()) return {};
    
    vector<string> tokens;
    std::size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

/**
 * @brief Replace occurrences of a character in a string with another character.
 *
 * @param s The input string by value (modified copy).
 * @param c Character to replace. If '\0', operation is a no-op.
 * @param with Replacement character. If '\0', operation is a no-op.
 * @return std::string Modified copy of the input string.
 */
inline std::string replace(std::string s, char c, char with)
{
    if( c == '\0' || with == '\0')
        return s;
    for(auto& ch: s)
        if(ch == c) 
            ch = with;
    return s;
}

/**
 * @brief Convert a vector of printable elements to a human-readable string.
 *
 * @tparam T Type of elements (must be convertible to std::string or formattable).
 * @param vec Vector of elements to stringify.
 * @return std::string A representation like "[ a, b, c ]".
 *
 * @details
 * The template requires that T is either convertible to std::string or
 * satisfies std::formattable<T, char> so that it can be appended to the result.
 */
template<class T>
requires std::convertible_to<T, std::string> || std::formattable<T, char>
std::string to_string(const std::vector<T>& vec) {
    std::string result = "[ ";
    for (std::size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i != vec.size() - 1) {
            result += ", ";
        }
    }
    result += " ]";
    return result;
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
    return to_hex(data, 1);
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
inline auto to_hex(const T* data, std::size_t n) -> std::string
{
    auto result = std::stringstream{};
    auto data_as_byte = reinterpret_cast<const unsigned char*>(data);
    std::size_t total_bytes = n * sizeof(T);

    for (std::size_t i = 0; i < total_bytes; ++i) {
        unsigned char byte = data_as_byte[i];
        result << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<int>(byte) << ' ';
       if ((i + 1) % 8 == 0) result << "  ";
    }
    return result.str();
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
 * - If the pointee is formattable -> returns `std::format("{}", *ptr)`.
 * - If the pointee is `void` -> returns an address formatted as `&lt;{:p}&gt; : void*`.
 * - Otherwise returns formatted `&lt;{:p}&gt; (size) : [ hex-dump ]`.
 *
 */
auto pointer_to_string(Pointer auto ptr) -> std::string
{
    using Pointee = std::remove_cv_t<std::remove_pointer_t<decltype(ptr)>>;

    if (ptr == nullptr) return "null";
    if constexpr (std::is_constructible_v<std::string, decltype(ptr)>) return std::string(ptr);

    if constexpr (std::is_function_v<Pointee>) {
        return ::type_name<Pointee>();
    } else if constexpr (std::formattable<Pointee, char>) {
        return std::format("{}", *ptr);
    } else if constexpr (std::is_same_v<Pointee, void>) {
        return std::format("<{:p}> : void*", static_cast<const void*>(ptr));
    } else {
        if constexpr (std::is_pointer_v<Pointee>) return pointer_to_string(*ptr);
        return std::format("<{:p}> ({}) : [ {} ]", static_cast<const void*>(ptr), sizeof(Pointee), to_hex(ptr));
    }
}

/**
 * @brief Platform-agnostic runtime loader for function addresses from a module.
 *
 * @param module Platform-dependent module name or library path (e.g. "opengl32.dll" or "libGL.so").
 * @param name Name of the symbol/function to resolve.
 * @return void* Address of the resolved symbol.
 *
 * @throws Exception if the module cannot be loaded or the symbol cannot be resolved.
 *
 * @details
 * - On Windows, attempts GetModuleHandleA(module) then LoadLibraryA(module) if needed,
 *   and resolves the symbol with GetProcAddress.
 * - On Linux, attempts dlopen(module, RTLD_LAZY | RTLD_NOLOAD) and resolves with dlsym.
 * - On Web (Emscripten), uses emscripten_webgl_get_proc_address.
 *
 * The function throws a Exception describing the failure reason if the module
 * cannot be loaded or the symbol cannot be found.
 */
inline auto get_proc_address([[maybe_unused]] const char* module, const char* name) -> void* {

    void* lib = nullptr;
    void* address = nullptr;
    std::string failreson;

    #if defined(WINDOWS_PLT)
    lib = [&module](){
        void* handle = GetModuleHandleA(module);
        return handle ? handle : LoadLibraryA(module);
    }();
    
    failreson = lib ? "" : std::to_string(GetLastError());
    address = (void *) ::GetProcAddress(reinterpret_cast<HMODULE>(lib), name);
    #elif defined(LINUX_PLT)
    lib = dlopen(module, RTLD_LAZY | RTLD_NOLOAD);
    failreson = lib ? "" : std::string(dlerror());
    address = (void *)dlsym(lib, name);
    #elif defined(WEB_PLT)
    address = reinterpret_cast<void*>(emscripten_webgl_get_proc_address(name));
    failreson = lib ? "" : reinterpret_cast<const char*>(emscripten_webgl_get_proc_address(name));
    #endif

    if(lib == nullptr){
        throw Exception("Couldn't load lib {} reason: {}, fn name: {}", module, failreson, name);
    }

    return address;
}

template<Variant TVarinat, class... TMatchers>
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

}// namespaceutils
