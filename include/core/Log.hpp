#pragma once

namespace config {
  constexpr auto LogFileName = "Engine.log";
}

namespace {

inline std::string formatedTime() {
  const auto now = std::chrono::system_clock::now();
  return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
}


struct ClogPolicy {
  static std::ostream& get_stream() {
    return std::cout;
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

inline auto& get_logger_mutex() {
  static std::mutex mtx;
  return mtx;
}


#if defined(DEBUG) || defined(WEB_PLT)
using LOGPolicy = ClogPolicy;
#else
using LOGPolicy = FilePolicy;
#endif

template <typename ...Ts>
auto Log(
  [[maybe_unused]] const std::format_string<Ts...>& fmt,
  [[maybe_unused]] Ts&& ... ts) -> void
{
  auto Is_Testing_Enabled = std::getenv("TESTING_ENABLED");

  std::scoped_lock lock(get_logger_mutex());

  auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
  [[maybe_unused]] auto& out = LOGPolicy::get_stream();

  if(Is_Testing_Enabled == nullptr){
    out << std::format("{} : {}\n", formatedTime(), formatted_msg);
  }
  
}

}


class CoreException final : public std::runtime_error {
public:
  using clock = std::chrono::system_clock;

  template <typename... Ts>
  CoreException(
    const std::source_location& loc,
    const std::format_string<Ts...>& fmt,
    Ts&&... args
  )
    : std::runtime_error(std::format(fmt, std::forward<Ts>(args)...))
    , m_Location(loc)
    , m_Timestamp(clock::now())
    #ifdef __cpp_lib_stacktrace
    , m_Trace(std::stacktrace::current(1))
    #endif
  {}
  auto throwing_routine() const noexcept -> const char* { return m_Location.function_name(); }

  auto trace() const noexcept -> const char* { 
    #ifdef __cpp_lib_stacktrace
      return to_string(m_Trace).c_str();
    #else
      return "no_stack_trace";
    #endif

  }
  auto when()  const noexcept -> const char* { return std::format("{:%Y-%m-%d %H:%M:%OS}", m_Timestamp).c_str(); }
  auto where() const noexcept -> const char* { return std::format("{}:{}", m_Location.file_name(), m_Location.line()).c_str(); }
  auto what()  const noexcept -> const char* override { return runtime_error::what(); }

  auto all() const noexcept -> const char*
  {
    return std::format(
      "{} :\n"
      "\t-> what : `{}`\n"
      "Stack Trace ({} Frame): {{\n{}}}\n",
      when(), what(),
      #ifdef __cpp_lib_stacktrace
      m_Trace.size(), trace()
      #else
      "0", "no_stack_trace"
      #endif
    ).c_str();
  }

private:
  std::source_location m_Location;
  clock::time_point m_Timestamp;
  #ifdef __cpp_lib_stacktrace
  std::stacktrace m_Trace;
  #endif
};

template <typename... Ts>
inline void print(const std::format_string<Ts...>& fmt, Ts&&... ts) {
  Log(fmt, std::forward<Ts>(ts)...);
}

template <typename... Ts>
inline void Info(const std::format_string<Ts...>& fmt, Ts&&... ts) {
  Log(fmt, std::forward<Ts>(ts)...);
}

#ifndef CException
#define CException(...) CoreException(std::source_location::current(), __VA_ARGS__)
#endif

#ifndef Expect
#define Expect(cond, ...) do { if (!(cond)){ print("Expectation `{}` Failed", #cond); throw CException(__VA_ARGS__); } } while (0)
#endif
