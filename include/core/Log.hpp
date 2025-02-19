#pragma once

#include <source_location>
#include <format>
#include <iostream>
#include <chrono>
#include <string_view>
#include <cstdlib>
#include <stacktrace>
#include <sstream>
#include <optional>
#include <mutex>
#include <windows.h>
#include <dbghelp.h>

auto setup_crach_handler() -> void;
auto resolveSymbol(void* addr, HANDLE proc = GetCurrentProcess()) -> std::string;
auto PrintStackTrace(unsigned short skip = 0) -> void;

inline thread_local std::optional<std::stacktrace> g_safeStacktrace; //dnt forget to make this only in debuge mode

namespace {
[[maybe_unused]] auto is_system_symbol(const std::string_view& symbol) -> bool{
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

auto stack_trace_formated(const unsigned short skip = 2) -> std::optional<std::string>
{
  const auto trace = std::stacktrace::current(skip);
  auto msg = std::stringstream{};
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

  if(frame_size != 0)
  {
    msg << std::format("Stack trace ({} frames):\n", frame_size);

    for (short unsigned int i = 0;  i < frame_size; i++)
    {      
      const auto desc = trace[i].description();
      const auto file = trace[i].source_file();
      const auto line = trace[i].source_line();

      msg << std::format(" #{:<2} in {} ({}:{:<2})\n", frame_size - i, desc, file, line);
    }
    return msg.str();
  }else{
    return std::nullopt;
  }
}

enum class Log_LvL : char {
  ERR,
  WAR,
};

static std::string formatedTime() {
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
}

constexpr const char* levelToString(Log_LvL level) {
  switch (level) {
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
    auto msg = std::stringstream{};

    msg << std::format("{} : [{}] {}\n--> {}:{}\n", formatedTime(), lvl_str, formatted_msg, loc.file_name(), loc.line()) << std::endl;
    
    if constexpr (lvl == Log_LvL::ERR){
      const auto op = stack_trace_formated();
      if(op){
        msg << op.value();
        MessageBoxA(nullptr, msg.str().c_str(), "ERROR", MB_YESNO | MB_ICONWARNING );
      }
    }
    
    std::lock_guard lock(mutex_);
    *out << msg.rdbuf();

    if constexpr (lvl == Log_LvL::ERR) exit(EXIT_FAILURE);
  }

  private:
    inline static std::mutex mutex_;
};

// template <Log_LvL lvl, typename... Ts>
template <Log_LvL lvl, std::ostream* out, typename ...Ts>
ERRF(const std::format_string<Ts...>, Ts&& ...) -> ERRF<lvl, out, Ts...>;

}

namespace Log
{

template <typename ...Ts>
auto print(const std::format_string<Ts...> fmt, Ts&& ... ts) -> void
{
  std::cout << std::format("{} : {}\n", formatedTime(), std::format(fmt, std::forward<Ts>(ts)...));
}

template <typename ...Ts>
auto Info(const std::format_string<Ts...> fmt, Ts&& ... ts) -> void
{
  std::cout << std::format("{} : [INFO] {}\n", formatedTime(), std::format(fmt, std::forward<Ts>(ts)...));
}


template <typename ...Ts>
using Error = ERRF<Log_LvL::ERR, &std::cerr, Ts...>;

template <typename ...Ts>
using Warning = ERRF<Log_LvL::WAR, &std::clog, Ts...>;


template <typename ...Ts>
[[noreturn]] auto Expect(bool x, const std::format_string<Ts...> fmt, Ts&& ... ts) -> void
{
  [[maybe_unused]] auto msg = std::format("{}\n", std::format(fmt, std::forward<Ts>(ts)...));

  if( x != true){
    print("Expection Failed : {} ", msg);
    PrintStackTrace();
    std::exit(1);
  }
}
} // namespace Log