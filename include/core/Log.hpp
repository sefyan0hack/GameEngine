#pragma once

#include <core/gl.h>

#if defined(WINDOWS_PLT)
#include <windows.h>
#ifdef DEBUG
#include <dbghelp.h>
auto setup_crach_handler() -> void;
auto resolveSymbol(void* addr, HANDLE proc = GetCurrentProcess()) -> std::string;
auto PrintStackTrace(unsigned short skip = 0) -> void;
#endif // DEBUG
#endif

namespace config {
  constexpr auto LogFileName = "Engine.log";
  inline bool exit_on_error = true;
  inline bool show_message_box = true;
  inline bool show_output = true;
  
  inline void TestFlags() {
    exit_on_error = false;
    show_message_box = false;
    show_output = false;
  }
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


struct FilePolicy {
  static auto& get_stream() {
    static std::ios_base::Init io_init;
    static std::ofstream stream;
    if (!stream.is_open()) {
      stream.open(config::LogFileName, std::ios::app);
      if (!stream) {
        std::abort();
      }
    }
    return stream;
  }
};

template<class T>
concept StreamOut = 
  std::is_base_of_v<
    std::basic_ostream<typename std::remove_cvref_t<T>::char_type, 
    typename std::remove_cvref_t<T>::traits_type>, 
    std::remove_cvref_t<T>
  >;

inline auto& get_logger_mutex() {
  static std::mutex mtx;
  return mtx;
}

template <Log_LvL lvl, typename ...Ts>
auto Log(
  [[maybe_unused]] StreamOut auto& out,
  [[maybe_unused]] std::source_location loc,
  [[maybe_unused]] const std::format_string<Ts...> fmt,
  [[maybe_unused]] Ts&& ... ts) -> void
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
      #ifdef HAS_STD_STACKTRACE
      std::stacktrace::current(1)
      #else
      "no_stack_trace"
      #endif
    );
    
    #if defined(WINDOWS_PLT)
    if (config::show_message_box){
      MessageBoxA(nullptr, msg.str().c_str(), "ERROR", MB_YESNO | MB_ICONERROR);
    }
    #endif
  }
  else if constexpr (lvl == Log_LvL::INFO){
    msg << std::format("{} : [{}] ``{}``\n", formatedTime(), lvl_str, formatted_msg);
  }
  else if constexpr (lvl == Log_LvL::PRT){
    msg << std::format("{} : {}\n", formatedTime(), formatted_msg);
  }

  if (config::show_output && out.good()){
    out << msg.view();
    out.flush();
  }

  if constexpr (lvl == Log_LvL::ERR  || lvl == Log_LvL::EXPT){
    if (config::exit_on_error) std::exit(EXIT_FAILURE);
  }
}

}

#ifdef DEBUG
#define Error(...) Log<Log_LvL::ERR>(std::cerr, std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO>(std::clog, std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT>(std::clog, std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation `{}` Failed", #cond); Log<Log_LvL::EXPT>(CerrPolicy::get_stream(), std::source_location::current(), __VA_ARGS__); } } while (0)

#else
#define Error(...) Log<Log_LvL::ERR>(FilePolicy::get_stream(), std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO>(FilePolicy::get_stream(), std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT>(FilePolicy::get_stream(), std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation `{}` Failed", #cond); Log<Log_LvL::EXPT>(FilePolicy::get_stream(), std::source_location::current(), __VA_ARGS__); } } while (0)
#endif
