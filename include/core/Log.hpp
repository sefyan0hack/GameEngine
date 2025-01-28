#pragma once

#include <source_location>
#include <format>
#include <iostream>
#include <chrono>
#include <string_view>
#include <cstdlib>

namespace {

enum class Log_LvL {
  ERR,
  WAR,
  INF,
};

static std::string formatTime(const std::chrono::system_clock::time_point& tp) {
  return std::format("{:%Y-%m-%d %H:%M:%OS}", tp);
}

static std::string_view levelToString(Log_LvL level) {
    switch (level) {
        case Log_LvL::INF: return "INFO";
        case Log_LvL::WAR: return "WARNING";
        case Log_LvL::ERR: return "ERROR";
        default:           return "UNKNOWN";
    }
}

template <Log_LvL lvl, typename ...Ts>
struct ERRF
{
  ERRF(const std::format_string<Ts...> fmt, Ts&& ... ts, std::source_location loc = std::source_location::current())
  {
    const auto now = std::chrono::system_clock::now();
    [[maybe_unused]] auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    auto lvl_str = levelToString(lvl);

    std::cout << std::format("[{}] {} : {}\n\t --> {}:{}", lvl_str, formatTime(now), formatted_msg, loc.file_name(), loc.line()) << "\n";
    if constexpr (lvl == Log_LvL::ERR){
      exit(EXIT_FAILURE);
    }
  }

};

template <Log_LvL lvl, typename... Ts>
ERRF(const std::format_string<Ts...>, Ts&& ...) -> ERRF<lvl, Ts...>;

}

namespace Log
{

template <typename ...Ts>
struct print{
  print(const std::format_string<Ts...> fmt, Ts&& ... ts)
  {
    const auto now = std::chrono::system_clock::now();
    [[maybe_unused]] auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);

    std::cout << std::format("{} : {}", formatTime(now), formatted_msg) << "\n";
  }
};

template <typename... Ts>
print(const std::format_string<Ts...>, Ts&& ...) -> print<Ts...>;


template <typename ...Ts>
using Error = ERRF<Log_LvL::ERR, Ts...>;

template <typename ...Ts>
using Warning = ERRF<Log_LvL::WAR, Ts...>;

template <typename ...Ts>
using Info = ERRF<Log_LvL::INF, Ts...>;

} // namespace Log