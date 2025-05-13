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
    extern bool show_output;
    extern bool use_exception;

    void TestFlags();
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
    static std::ofstream stream;
    static bool initialized = false;
    static bool open_failed = false;

    if (!initialized) {
      initialized = true;
      stream.open(config::LogFileName, std::ios::app);
      if (!stream.is_open()) {
        std::cerr << "Couldn't open file: " << config::LogFileName << std::endl;
        open_failed = true;
      }
    }

    return open_failed ? std::clog : stream;
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
      std::stacktrace::current(1)
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
  if(!config::show_output) return;

  auto& out = Out::get_stream();
  auto msg = Log_msg<lvl>(loc, fmt, std::forward<Ts>(ts)...);

  if(!config::use_exception) out << msg;

  if constexpr (lvl == Log_LvL::ERR || lvl == Log_LvL::EXPT){
    if(!config::use_exception){
      std::terminate();
    }else{
      throw std::runtime_error(msg);
    }
  }
}

}

#ifdef DEBUG
#define Error(...) Log<Log_LvL::ERR, CerrPolicy>(std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO, ClogPolicy>(std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT, ClogPolicy>(std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation `{}` Failed", #cond); Log<Log_LvL::EXPT, ClogPolicy>(std::source_location::current(), __VA_ARGS__); } } while (0)

#else
#define Error(...) Log<Log_LvL::ERR, FilePolicy>(std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO, FilePolicy>(std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT, FilePolicy>(std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation `{}` Failed", #cond); Log<Log_LvL::EXPT, FilePolicy>(std::source_location::current(), __VA_ARGS__); } } while (0)
#endif