#pragma once

#include <source_location>
#include <format>
#include <iostream>
#include <chrono>
#include <string_view>
#include <cstdlib>
#include <stacktrace>
#include <sstream>
#include <fstream>
#include <optional>
#include <mutex>
#include <windows.h>
#include <dbghelp.h>
#include <core/gl.h>

auto setup_crach_handler() -> void;
auto resolveSymbol(void* addr, HANDLE proc = GetCurrentProcess()) -> std::string;
auto PrintStackTrace(unsigned short skip = 0) -> void;

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

enum class Log_LvL : char {
  ERR,
  INFO,
};

static std::string formatedTime() {
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
}

constexpr const char* levelToString(Log_LvL level) {
  switch (level) {
    case Log_LvL::INFO: return "INFO";
    case Log_LvL::ERR:  return "ERROR";
    default:            return "UNKNOWN";
  }
}

struct CerrPolicy {
  static auto& get_stream() {
      static auto& stream = std::cerr;
      return stream;
  }
};

struct ClogPolicy {
    static auto& get_stream() {
        static auto& stream = std::clog;
        return stream;
    }
};

struct FilePolicy {
    static auto& get_stream() {
        static std::ofstream stream("Engine.log", std::ios::app);
        return stream;
    }
};

template<class T>
concept StreamOut = requires(){
  { T::get_stream() } -> std::convertible_to<std::ostream&>;
};

template <Log_LvL lvl, StreamOut Out, typename ...Ts>
struct ERRF
{
  ERRF([[maybe_unused]] const std::format_string<Ts...> fmt, [[maybe_unused]] Ts&& ... ts, [[maybe_unused]] std::source_location loc = std::source_location::current())
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    auto lvl_str = levelToString(lvl);
    auto msg = std::stringstream{};
    auto& out = Out::get_stream();


    if constexpr (lvl == Log_LvL::INFO)
    {
      msg << std::format("{} : [{}] {}", formatedTime(), lvl_str, formatted_msg) << "\n";
    }
    else if constexpr (lvl == Log_LvL::ERR)
    {
      msg << std::format("{} : [{}] {}\n--> {}:{}", formatedTime(), lvl_str, formatted_msg, loc.file_name(), loc.line())
      << "\n" << std::stacktrace::current(1) << "\n";
      MessageBoxA(nullptr, msg.str().c_str(), "ERROR", MB_YESNO | MB_ICONWARNING );
    }

    {
      std::lock_guard lock(mutex_);
      out << msg.rdbuf();
      out.flush();
    }

    if constexpr (lvl == Log_LvL::ERR) exit(EXIT_FAILURE);
  }

  private:
    inline static std::mutex mutex_;
};

template <Log_LvL lvl, StreamOut Out, typename ...Ts>
ERRF(const std::format_string<Ts...>, Ts&& ...) -> ERRF<lvl, Out, Ts...>;

}

namespace Log
{

template <typename ...Ts>
auto print(const std::format_string<Ts...> fmt, Ts&& ... ts) -> void
{
  std::cout << std::format("{} : {}\n", formatedTime(), std::format(fmt, std::forward<Ts>(ts)...));
}



template <typename ...Ts>
using Error = ERRF<Log_LvL::ERR, CerrPolicy, Ts...>;

#ifdef DEBUG
template <typename ...Ts>
using Info = ERRF<Log_LvL::INFO, ClogPolicy, Ts...>;
#else
template <typename ...Ts>
using Info = ERRF<Log_LvL::INFO, FilePolicy, Ts...>;
#endif

template <typename ...Ts>
auto Expect(bool x, const std::format_string<Ts...> fmt, Ts&& ... ts) -> void
{
  [[maybe_unused]] auto msg = std::format("{}\n", std::format(fmt, std::forward<Ts>(ts)...));

  if( x != true){
    print("Expection Failed : {} ", msg);
    std::cerr << std::stacktrace::current(1) << std::endl;
    std::abort();
  }
}
} // namespace Log