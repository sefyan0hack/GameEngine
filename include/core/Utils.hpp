#pragma once

#include <thread>
#include <chrono>
#include <functional>
#include <array>
#include <cstddef>
#include <string_view>
#include <algorithm>

template<typename Function, typename... Args>
auto setTimeOut( unsigned long delay, Function&& func, Args&&... args) -> void
{
    std::thread([func, delay, args...]() mutable {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        std::invoke(func, std::forward<Args>(args)...);
    }).detach();
}

// compile time
constexpr auto get_file_directory(const std::string_view file_path = __FILE__) -> std::string_view
{
    const size_t last_slash = file_path.find_last_of("/\\");
    return (last_slash == std::string_view::npos) ? "" : file_path.substr(0, last_slash);
}

constexpr auto is_odd(auto x) -> bool
{
    return x % 2 != 0;
}

template <std::size_t N>
struct FixedString {
    constexpr FixedString(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    char value[N];
};

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

#define TO_UPPER(str) std::string_view(ToUpper<FixedString{str}>::value.data())