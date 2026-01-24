#pragma once
#include <string>
#include <string_view>
#include <concepts>
#include <tuple>

#ifdef __has_include
  #if __has_include(<format>)
    #include <format>
    template <typename... Args>
    std::string format(std::format_string<Args...> fmt, Args&&... args) {
        [[maybe_unused]] auto to_safe_arg = [](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (formattable<T>) {
                return std::cref(arg);
            } else {
                return std::format("[Unformattable: {}]", ::type_name<T>());
            }
        };

        auto safe_args = std::make_tuple(to_safe_arg(args)...);

        return std::apply([&fmt](const auto&... unpacked_args) {
            return std::vformat(fmt.get(), std::make_format_args(unpacked_args...));
        }, safe_args);
    }
  #endif
#endif