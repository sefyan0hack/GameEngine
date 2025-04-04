#pragma once

#include <core/gl.h>

#ifdef _WIN32
#ifdef DEBUG
auto setup_crach_handler() -> void;
auto resolveSymbol(void* addr, HANDLE proc = GetCurrentProcess()) -> std::string;
auto PrintStackTrace(unsigned short skip = 0) -> void;
#endif //DEBUG
#endif

namespace {

enum class Log_LvL : char {
  ERR,
  INFO,
  EXPT,
  PRT,
};

static std::string formatedTime() {
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

inline auto& get_logger_mutex() {
  static std::mutex mtx;
  return mtx;
}

template <Log_LvL lvl, StreamOut Out, typename ...Ts>
auto Log(
  [[maybe_unused]] std::source_location loc,
  [[maybe_unused]] const std::format_string<Ts...> fmt,
  [[maybe_unused]] Ts&& ... ts) -> void
{
  std::scoped_lock lock(get_logger_mutex());
  auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
  auto lvl_str = levelToString(lvl);
  auto msg = std::stringstream{};
  auto& out = Out::get_stream();
  
  if constexpr (lvl == Log_LvL::ERR){
    msg << std::format("{} : [{}] {}\n--> {}:{}", formatedTime(), lvl_str, formatted_msg, loc.file_name(), loc.line())
    << "\n" << std::stacktrace::current(1) << "\n";

    #ifdef _WIN32
    MessageBoxA(nullptr, msg.str().c_str(), "ERROR", MB_YESNO | MB_ICONERROR );
    #endif
  }
  else if constexpr (lvl == Log_LvL::INFO){
    msg << std::format("{} : [{}] {}", formatedTime(), lvl_str, formatted_msg) << "\n";
  }
  else if constexpr (lvl == Log_LvL::EXPT){
    msg << std::format("{} : [{}] {}\n--> {}:{}", formatedTime(), lvl_str, formatted_msg, loc.file_name(), loc.line())
    << "\n" << std::stacktrace::current(1) << "\n";

    #ifdef _WIN32
    MessageBoxA(nullptr, msg.str().c_str(), "ERROR", MB_YESNO | MB_ICONWARNING );
    #endif
  }
  else if constexpr (lvl == Log_LvL::PRT){
    msg << std::format("{} : {}\n", formatedTime(), formatted_msg);
  }

  {
    out << msg.rdbuf();
    out.flush();
  }

  if constexpr (lvl == Log_LvL::ERR  or lvl == Log_LvL::EXPT) exit(EXIT_FAILURE);
}

}

#ifdef DEBUG
#define Error(...) Log<Log_LvL::ERR, CerrPolicy>(std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO, ClogPolicy>(std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT, ClogPolicy>(std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation Failed: "#cond); Log<Log_LvL::EXPT, ClogPolicy>(std::source_location::current(), __VA_ARGS__); } } while (0)

#else
#define Error(...) Log<Log_LvL::ERR, FilePolicy>(std::source_location::current(), __VA_ARGS__)
#define Info(...)  Log<Log_LvL::INFO, FilePolicy>(std::source_location::current(), __VA_ARGS__)
#define print(...) Log<Log_LvL::PRT, FilePolicy>(std::source_location::current(), __VA_ARGS__)
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation Failed: "#cond); Log<Log_LvL::EXPT, FilePolicy>(std::source_location::current(), __VA_ARGS__); } } while (0)
#endif