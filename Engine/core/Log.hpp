#pragma once

#include <format>
#include <source_location>
#include <engine_export.h>

namespace debug {
    auto log(const char* msg) -> void;
    auto log(std::string msg) -> void;

    [[noreturn]] auto unimpl(std::source_location loc = std::source_location::current()) noexcept -> void;

    template <class... Ts>
    inline auto log(const std::format_string<Ts...>& fmt, Ts&&... ts) -> void
    {
        log(std::format(fmt, std::forward<Ts>(ts)...));
    }

    template <class T>
    inline auto log(T&& x) -> void
    {
        debug::log("{}", std::forward<T>(x));
    }

    #define THIS_STR std::format("{}", *this)
    #define THIS_LOG debug::log("{}", *this)
    #define THIS_ADDR (const void*)this

    // #define CTOR_LOG 
    // #define DTOR_LOG 
    #define CTOR_LOG debug::log("({:p})  {} {}", (const void*)this, ::type_name<std::remove_pointer_t<decltype(this)>>(), *this);
    #define DTOR_LOG debug::log("({:p}) ~{}", (const void*)this, ::type_name<std::remove_pointer_t<decltype(this)>>());

} // namespace debug