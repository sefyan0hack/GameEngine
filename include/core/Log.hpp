#pragma once

#include <source_location>
#include <format>
#include <iostream>
#include <cstdlib>
#include <string_view>

namespace Log
{

enum class Log_LvL {
  ERR,
  WAR,
  INF,
  PRT
};

template <Log_LvL lvl, typename ...Ts>
struct ERRF
{
  ERRF(const std::format_string<Ts...> fmt, Ts&& ... ts, std::source_location loc = std::source_location::current())
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);

    if constexpr (lvl == Log_LvL::ERR){
      std::cout << std::format("<X> [ERROR] {}:{} -> {}", loc.file_name(), loc.line(), formatted_msg) << "\n";
      std::abort();
    }else if constexpr (lvl == Log_LvL::INF){
      std::cout << std::format("<?> [INFO] {}:{} -> {}", loc.file_name(), loc.line(), formatted_msg) << "\n";

    }else if constexpr (lvl == Log_LvL::WAR){
      std::cout << std::format("<!> [WARNING] {}:{} -> {}", loc.file_name(), loc.line(), formatted_msg) << "\n";

    }else if constexpr (lvl == Log_LvL::PRT){
      std::cout << formatted_msg << "\n";
    }
  }
};

template <Log_LvL lvl, typename... Ts>
ERRF(const std::format_string<Ts...>, Ts&& ...) -> ERRF<lvl, Ts...>;

template <typename ...Ts>
using Error = ERRF<Log_LvL::ERR, Ts...>;

template <typename ...Ts>
using Warning = ERRF<Log_LvL::WAR, Ts...>;

template <typename ...Ts>
using Info = ERRF<Log_LvL::INF, Ts...>;

template <typename ...Ts>
using print = ERRF<Log_LvL::PRT, Ts...>;

} // namespace Log