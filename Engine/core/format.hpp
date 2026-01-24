#pragma once
#include <string>
#include <string_view>
#include <concepts>
#include <tuple>

#ifdef __has_include
  #if __has_include(<format>)
    #include <format>

    #ifdef __cpp_lib_formatters
        template <typename... Args>
        std::string format(std::format_string<Args...> fmt, Args&&... args) { return std::format(fmt, std::forward<Args>(args)...); }
    #else
    template<typename T>
    requires std::formattable<T, char>
    constexpr decltype(auto) make_safe_arg(T&& v) {
        return std::forward<T>(v);
    }

    template<typename T>
    requires (!std::formattable<T, char>)
    std::string make_safe_arg(T&&) {
        return std::format("[Unformattable: {}]", ::type_name<std::decay_t<T>>());
    }

    template <typename... Args>
    std::string format(std::format_string<Args...> fmt, Args&&... args) {
        [[maybe_unused]] auto safe_args = std::tuple{ make_safe_arg(std::forward<Args>(args))... };

        return std::apply(
            [&](auto&... unpacked) {
                return std::vformat(
                    fmt.get(),
                    std::make_format_args(unpacked...)
                );
            },
            safe_args
        );
    }
    #endif
  #endif
#endif