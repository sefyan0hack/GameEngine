#pragma once

#if defined(WINDOWS_PLT)
#include <windows.h>
#ifdef DEBUG
auto setup_crach_handler() -> void;
auto resolveSymbol(void* addr, HANDLE proc = GetCurrentProcess()) -> std::string;
auto PrintStackTrace(unsigned short skip = 0) -> void;
#endif //DEBUG
#endif

namespace config {
    constexpr auto LogFileName = "Engine.log";
}
namespace {

enum class Log_LvL : char {
  ERR,
  INFO,
  EXPT,
  PRT,
};

inline std::string formatedTime() {
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
}

constexpr const char* levelToString(Log_LvL level) {
  switch (level) {
    case Log_LvL::ERR:  return "ERROR";
    case Log_LvL::INFO: return "INFO";
    case Log_LvL::EXPT: return "EXPECT";
    case Log_LvL::PRT:  return "PRINT";
    default:            return "UNKNOWN";
  }
}

struct CerrPolicy {
  static std::ostream& get_stream() {
    return std::cerr;
  }
};

struct ClogPolicy {
  static std::ostream& get_stream() {
    return std::clog;
  }
};

struct FilePolicy {
  static std::ostream& get_stream() {
    static std::unique_ptr<std::ofstream> stream;
    static std::once_flag once_flag;
    static bool open_failed = false;

    std::call_once(once_flag, []() {
      static std::ios_base::Init init_guard;
      stream = std::make_unique<std::ofstream>(
        config::LogFileName, 
        std::ios::app
      );
      if (!stream->is_open()) {
        std::cerr << "Failed to open log file: " << config::LogFileName << "\n";
        open_failed = true;
        stream.reset();
      }
    });

    return (open_failed || !stream) ? std::clog : *stream;
  }
};

template<class T>
concept StreamOut = requires(){
  { T::get_stream() } -> std::convertible_to<std::ostream&>;
};

inline auto& get_logger_mutex() {
  static std::mutex mtx;
  return mtx;
}

template <Log_LvL lvl, typename ...Ts>
auto Log_msg(
  [[maybe_unused]] const std::source_location loc,
  [[maybe_unused]] const std::format_string<Ts...>& fmt,
  [[maybe_unused]] Ts&& ... ts) -> std::string
{
  std::scoped_lock lock(get_logger_mutex());
  auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
  auto lvl_str = levelToString(lvl);
  auto msg = std::stringstream{};
  
  if constexpr (lvl == Log_LvL::ERR || lvl == Log_LvL::EXPT){
    msg << std::format(
      "{} : [{}] ``{}``\n"
      "\t-> `{}` [{}:{}]\n"
      "{}\n",
      formatedTime(), lvl_str, formatted_msg,
      loc.function_name(), loc.file_name(), loc.line(),
      #ifdef __cpp_lib_stacktrace
      std::stacktrace::current(2)
      #else
      "no_stack_trace"
      #endif
    );
  }
  else if constexpr (lvl == Log_LvL::INFO){
    msg << std::format("{} : [{}] ``{}``\n", formatedTime(), lvl_str, formatted_msg);
  }
  else if constexpr (lvl == Log_LvL::PRT){
    msg << std::format("{} : {}\n", formatedTime(), formatted_msg);
  }
  
  return msg.str();
}
template <Log_LvL lvl, StreamOut Out, typename ...Ts>
auto Log(
  [[maybe_unused]] const std::source_location loc,
  [[maybe_unused]] const std::format_string<Ts...>& fmt,
  [[maybe_unused]] Ts&& ... ts) -> void
{
  auto& out = Out::get_stream();
  auto msg = Log_msg<lvl>(loc, fmt, std::forward<Ts>(ts)...);
  
  if constexpr (lvl == Log_LvL::ERR || lvl == Log_LvL::EXPT){
    #ifndef DEBUG
    out << msg;
    #endif
    throw std::runtime_error(msg);
  }else{
    out << msg;
  }
}

}

#ifdef DEBUG
#define LOGPolicy ClogPolicy
#else
#define LOGPolicy FilePolicy
#endif


#define Error(...) Log<Log_LvL::ERR, LOGPolicy>(std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO, LOGPolicy>(std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT, LOGPolicy>(std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation `{}` Failed", #cond); Log<Log_LvL::EXPT, LOGPolicy>(std::source_location::current(), __VA_ARGS__); } } while (0)