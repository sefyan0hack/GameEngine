#pragma once

#include <format>
#include <source_location>
#include <string>
#include <engine_export.h>

namespace debug {
    using logger_handler_t = void(std::string msg);

    ENGINE_EXPORT logger_handler_t default_logger;
    ENGINE_EXPORT extern logger_handler_t* logger;

    ENGINE_EXPORT [[noreturn]] auto unimpl(std::source_location loc = std::source_location::current()) noexcept -> void;
    ENGINE_EXPORT [[noreturn]] auto unrechable(const char* msg = "", std::source_location loc = std::source_location::current()) noexcept -> void;

    template <class... Ts>
    inline auto log(const std::format_string<Ts...>& fmt, Ts&&... ts) -> void
    {
        if(logger) logger(std::format(fmt, std::forward<Ts>(ts)...));
    }

    template <class T>
    inline auto log(T&& x) -> void
    {
        debug::log("{}", std::forward<T>(x));
    }

} // namespace debug

#if defined(OBJ_LIFETIME_TRACE)
#   define CTOR_LOG debug::log("start [{:p}] {}", (const void*)this, typeid(*this).name());
#   define DTOR_LOG debug::log("end   [{:p}] {}", (const void*)this, typeid(*this).name());
#else
#   define CTOR_LOG
#   define DTOR_LOG
#endif