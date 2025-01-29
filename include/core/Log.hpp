#pragma once

#include <source_location>
#include <format>
#include <iostream>
#include <chrono>
#include <string_view>
#include <cstdlib>
#include <stacktrace>

namespace {
[[maybe_unused]] bool is_system_symbol(const std::string_view& symbol) {
    const std::string_view system_libraries[] = {
        "KERNEL32", "ntdll", "msvcrt", "ucrtbase", "vcruntime",
        "invoke_main", "__scrt_common_main", "mainCRTStartup"
    };

    for (const auto& lib : system_libraries) {
        if (symbol.find(lib) != std::string_view::npos) {
            return true;
        }
    }
    return false;
}

enum class Log_LvL {
  ERR,
  WAR,
  INF,
};

static std::string formatedTime() {
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
}

static std::string_view levelToString(Log_LvL level) {
  switch (level) {
    case Log_LvL::INF: return "INFO";
    case Log_LvL::WAR: return "WARNING";
    case Log_LvL::ERR: return "ERROR";
    default:           return "UNKNOWN";
  }
}

template <Log_LvL lvl, std::ostream* out, typename ...Ts>
struct ERRF
{
  ERRF(const std::format_string<Ts...> fmt, Ts&& ... ts, std::source_location loc = std::source_location::current())
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    auto lvl_str = levelToString(lvl);

    *out << std::format("[{}] {} : {}\n--> {}:{}\n", lvl_str, formatedTime(), formatted_msg, loc.file_name(), loc.line()) << "\n";
    
    if constexpr (lvl == Log_LvL::ERR){
      const auto trace = std::stacktrace::current(1);
      short unsigned int frame_size = 0;

      // filter system function on call stack
      for (const auto& entry : trace)
      {
        #ifdef _MSC_VER
        if( is_system_symbol(entry.description()) || entry.source_line() == 0) break;
        #else
        if( entry.source_line() == 0) break;
        #endif

        frame_size++;
      }

      *out << std::format("Stack trace ({} frames):\n", frame_size);

      for (short unsigned int i = 0;  i < frame_size; i++)
      {      
        const auto desc = trace[i].description();
        const auto file = trace[i].source_file();
        const auto line = trace[i].source_line();

        *out << std::format("  #{:<2} {}:{:<2} :: {} \n", frame_size - i, file, line, desc);
      }

      exit(EXIT_FAILURE);
    }
  }
};

// template <Log_LvL lvl, typename... Ts>
template <Log_LvL lvl, std::ostream* out, typename ...Ts>
ERRF(const std::format_string<Ts...>, Ts&& ...) -> ERRF<lvl, out, Ts...>;

}

namespace Log
{

template <typename ...Ts>
struct print{
  print(const std::format_string<Ts...> fmt, Ts&& ... ts)
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    std::cout << std::format("{} : {}", formatedTime(), formatted_msg) << "\n";
  }
};

template <typename... Ts>
print(const std::format_string<Ts...>, Ts&& ...) -> print<Ts...>;


template <typename ...Ts>
using Error = ERRF<Log_LvL::ERR, &std::cerr, Ts...>;

template <typename ...Ts>
using Warning = ERRF<Log_LvL::WAR, &std::clog, Ts...>;

template <typename ...Ts>
using Info = ERRF<Log_LvL::INF, &std::clog, Ts...>;

} // namespace Log