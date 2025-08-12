#pragma once

#include <source_location>
#include <stacktrace>
#include <iostream>
#include <cstdlib>
#include <format>
#include <chrono>
#include <exception>

namespace Debug {

  inline std::string formatedTime() {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
  }

  template <typename ...Ts>
  auto Log(
    [[maybe_unused]] const std::format_string<Ts...>& fmt,
    [[maybe_unused]] Ts&& ... ts) -> void
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);

    if(std::getenv("TESTING_ENABLED") == nullptr){
      std::cout << std::format("{} : {}\n", formatedTime(), formatted_msg);
    }
  }
  

  template <typename... Ts>
  inline auto Print([[maybe_unused]] const std::format_string<Ts...>& fmt, [[maybe_unused]] Ts&&... ts) -> void
  {
    #if defined(DEBUG) && !defined(NDEBUG)
    Log(fmt, std::forward<Ts>(ts)...);
    #endif
  }

} // namespace Debug


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

#ifndef CException
#define CException(...) CoreException(std::source_location::current(), __VA_ARGS__)
#endif

#ifndef Expect
#define Expect(cond, ...) do { if (!(cond)){ Debug::Print("Expectation `{}` Failed", #cond); throw CException(__VA_ARGS__); } } while (0)
#endif
