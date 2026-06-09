#pragma once

#include <format>
#include <source_location>
#include <string>
#include <engine_export.h>

namespace logg {
    constexpr enum class LogLvl { None, Error, Info, Warn, Trace } lvl = LogLvl::Warn;

    using logger_handler_t = void(LogLvl l, std::string msg);

    ENGINE_EXPORT logger_handler_t default_logger;
    ENGINE_EXPORT extern logger_handler_t* logger;

    template <class... Ts>
    inline auto error(const std::format_string<Ts...>& fmt, Ts&&... ts) -> void
    {
        if(logger) logger(LogLvl::Error, std::format(fmt, std::forward<Ts>(ts)...));
    }

    template <class... Ts>
    inline auto info(const std::format_string<Ts...>& fmt, Ts&&... ts) -> void
    {
        if(logger) logger(LogLvl::Info, std::format(fmt, std::forward<Ts>(ts)...));
    }

    template <class... Ts>
    inline auto warn(const std::format_string<Ts...>& fmt, Ts&&... ts) -> void
    {
        if(logger) logger(LogLvl::Warn, std::format(fmt, std::forward<Ts>(ts)...));
    }

    template <class... Ts>
    inline auto trace(const std::format_string<Ts...>& fmt, Ts&&... ts) -> void
    {
        if(logger) logger(LogLvl::Trace, std::format(fmt, std::forward<Ts>(ts)...));
    }

    template <class T>
    inline auto info(T&& x) -> void
    {
        logg::info("{}", std::forward<T>(x));
    }

    template <class T>
    inline auto warn(T&& x) -> void
    {
        logg::warn("{}", std::forward<T>(x));
    }

    template <class T>
    inline auto trace(T&& x) -> void
    {
        logg::trace("{}", std::forward<T>(x));
    }

    template <class T>
    inline auto error(T&& x) -> void
    {
        logg::error("{}", std::forward<T>(x));
    }

    ENGINE_EXPORT [[noreturn]] auto unimpl(std::source_location loc = std::source_location::current()) noexcept -> void;
    ENGINE_EXPORT [[noreturn]] auto unrechable(const char* msg = "", std::source_location loc = std::source_location::current()) noexcept -> void;

} // namespace log